//! Record actual solver outcomes and check merge, retry and restore behavior.
//! Two local forks simulate writers. This is not a network scheduler or GPU lock.
use serde_json::{json, Value};
use std::{env, fs, io::Write, path::Path};
use zergraph::{EdgeKey, Graph, Snapshot};

fn record(graph: &mut Graph, record: &Value) -> Result<(), Box<dyn std::error::Error>> {
    let id = record["id"].as_str().ok_or("Missing record id")?;
    graph.add_node(id)?;
    for (key, value) in record["properties"]
        .as_object()
        .ok_or("Missing properties")?
    {
        graph.set_node_property(id, key, value.clone())?;
    }
    for edge in record["edges"].as_array().ok_or("Missing edges")? {
        let label = edge[0].as_str().ok_or("Missing edge label")?;
        let target = edge[1].as_str().ok_or("Missing edge target")?;
        graph.add_node(target)?;
        graph.add_edge(EdgeKey::new(id, label, target))?;
    }
    Ok(())
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<_> = env::args().collect();
    if args.len() != 3 {
        return Err("Usage: solver-evidence-board INPUT.json NEW.snapshot".into());
    }
    let input: Value = serde_json::from_slice(&fs::read(&args[1])?)?;
    let mut base = Graph::new();
    for row in input["shared"].as_array().ok_or("Missing shared records")? {
        record(&mut base, row)?;
    }
    let mut small = base.fork();
    let mut large = base.fork();
    let observations = input["evaluations"]
        .as_array()
        .ok_or("Missing evaluations")?;
    for row in observations {
        let writer = if row["properties"]["condition"]["width"] == 8 {
            &mut small
        } else {
            &mut large
        };
        record(writer, row)?;
    }
    let mut first = base.fork();
    first.merge_with_changes(&small.snapshot())?;
    first.merge_with_changes(&large.snapshot())?;
    let mut second = base.fork();
    second.merge_with_changes(&large.snapshot())?;
    second.merge_with_changes(&small.snapshot())?;
    assert_eq!(first.snapshot(), second.snapshot());
    assert!(first.merge_with_changes(&small.snapshot())?.is_empty());
    let bytes = first.snapshot().to_bytes()?;
    let restored = Graph::from_snapshot(Snapshot::from_bytes(&bytes)?);
    assert_eq!(restored.snapshot(), first.snapshot());
    // Check each recorded metric, including failures, after merging and restore.
    for row in observations {
        let node = restored
            .node(row["id"].as_str().unwrap())
            .ok_or("Lost observation")?;
        assert_eq!(
            node.property("metrics"),
            Some(&row["properties"]["metrics"])
        );
    }
    let output = Path::new(&args[2]);
    let mut file = fs::OpenOptions::new()
        .write(true)
        .create_new(true)
        .open(output)?;
    file.write_all(&bytes)?;
    file.sync_all()?;
    println!(
        "{}",
        json!({"observations_checked": observations.len(),
        "nodes": restored.nodes().count(), "edges": restored.edges().count(),
        "snapshot_bytes": bytes.len(), "reverse_order_merge": "passed",
        "duplicate_delivery": "passed", "snapshot_restore": "passed"})
    );
    Ok(())
}
