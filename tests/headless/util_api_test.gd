extends SceneTree

var _failures: Array[String] = []
var _test_root := "user://pdje_gdextension_headless"


func _initialize() -> void:
	call_deferred("_run")


func _run() -> void:
	var test_root_path := ProjectSettings.globalize_path(_test_root)
	var directory_error := DirAccess.make_dir_recursive_absolute(test_root_path)
	if directory_error != OK:
		push_error("Failed to create test root %s: %s" % [
			test_root_path, error_string(directory_error)])
		quit(1)
		return

	_test_registered_classes()
	_test_key_value_db()
	_test_relational_db()
	_test_vector_db()
	_test_stft()
	_test_beat_this()

	if _failures.is_empty():
		print("PDJE_GODOT_HEADLESS_TESTS_PASSED")
		quit(0)
		return

	for failure in _failures:
		push_error(failure)
	print("PDJE_GODOT_HEADLESS_TESTS_FAILED: %d" % _failures.size())
	quit(1)


func _expect(condition: bool, message: String) -> void:
	if not condition:
		_failures.append(message)


func _test_registered_classes() -> void:
	for registered_class in [
		"PDJE_KeyValueDB",
		"PDJE_VectorDB",
		"PDJE_VectorItem",
		"PDJE_VectorHit",
		"PDJE_RelationalDB",
		"PDJE_RelationalRow",
		"PDJE_RelationalExecResult",
		"PDJE_MIR",
		"PDJE_StftResult",
		"PDJE_AI",
		"PDJE_BeatThisDetector",
		"PDJE_BeatThisResult",
	]:
		_expect(ClassDB.class_exists(registered_class),
			"ClassDB is missing %s" % registered_class)


func _test_key_value_db() -> void:
	var path := _test_root + "/key_value"
	var db := PDJE_KeyValueDB.new()
	_expect(db.Create(path, true), "KeyValue Create failed")
	_expect(db.Open(path), "KeyValue Open failed")
	_expect(db.IsOpen(), "KeyValue did not report open")
	_expect(db.PutText("alpha:1", "hello"), "KeyValue PutText failed")
	_expect(db.GetText("alpha:1") == "hello", "KeyValue GetText mismatch")

	var bytes := PackedByteArray([1, 171, 0, 255])
	_expect(db.PutBytes("blob", bytes), "KeyValue PutBytes failed")
	_expect(db.GetBytes("blob") == bytes, "KeyValue GetBytes mismatch")
	_expect(db.Contains("alpha:1"), "KeyValue Contains failed")
	_expect(db.ListKeys("alpha:") == PackedStringArray(["alpha:1"]),
		"KeyValue prefix ListKeys mismatch")
	_expect(db.Erase("alpha:1"), "KeyValue Erase failed")
	_expect(not db.Contains("alpha:1"), "KeyValue erase was not persisted")
	_expect(db.Close(), "KeyValue Close failed")
	_expect(db.Destroy(path), "KeyValue Destroy failed")
	db.free()


func _test_relational_db() -> void:
	var path := _test_root + "/relational.sqlite3"
	var db := PDJE_RelationalDB.new()
	_expect(db.Create(path, true), "Relational Create failed")
	_expect(db.Open(path), "Relational Open failed")

	var schema = db.Execute(
		"CREATE TABLE items(id INTEGER PRIMARY KEY, name TEXT, payload BLOB)")
	_expect(schema != null, "Relational schema creation failed")
	var insert = db.Execute(
		"INSERT INTO items(id, name, payload) VALUES(?, ?, ?)",
		[7, "alpha", PackedByteArray([3, 1, 4])])
	_expect(insert != null, "Relational insert failed")
	if insert != null:
		_expect(insert.affected_rows == 1,
			"Relational affected_rows mismatch")

	var rows: Array = db.Query(
		"SELECT id, name, payload FROM items WHERE id = ?", [7])
	_expect(rows.size() == 1, "Relational query row count mismatch")
	if rows.size() == 1:
		var row = rows[0]
		_expect(row.columns == PackedStringArray(["id", "name", "payload"]),
			"Relational columns mismatch")
		_expect(row.values[0] == 7 and row.values[1] == "alpha",
			"Relational scalar values mismatch")
		_expect(row.values[2] == PackedByteArray([3, 1, 4]),
			"Relational blob mismatch")

	_expect(db.BeginTransaction(), "Relational begin transaction failed")
	_expect(db.Execute(
		"INSERT INTO items(id, name) VALUES(?, ?)", [8, "rollback"]) != null,
		"Relational transactional insert failed")
	_expect(db.Rollback(), "Relational rollback failed")
	_expect(db.Query("SELECT id FROM items WHERE id = ?", [8]).is_empty(),
		"Relational rollback did not remove the row")
	_expect(db.Close(), "Relational Close failed")
	_expect(db.Destroy(path), "Relational Destroy failed")
	db.free()


func _test_vector_db() -> void:
	var path := _test_root + "/vectors"
	var db := PDJE_VectorDB.new()
	_expect(db.Create(path, 3, 10, false, true), "Vector Create failed")
	_expect(db.Open(path, 3), "Vector Open failed")

	var alpha := PDJE_VectorItem.new()
	alpha.id = "alpha"
	alpha.embedding = PackedFloat32Array([1.0, 0.0, 0.0])
	alpha.text_payload = "first"
	_expect(db.UpsertItem(alpha), "Vector alpha upsert failed")

	var beta := PDJE_VectorItem.new()
	beta.id = "beta"
	beta.embedding = PackedFloat32Array([0.0, 1.0, 0.0])
	beta.bytes_payload = PackedByteArray([9, 8, 7])
	_expect(db.UpsertItem(beta), "Vector beta upsert failed")
	_expect(db.Contains("alpha"), "Vector Contains failed")

	var stored = db.GetItem("alpha")
	_expect(stored != null, "Vector GetItem returned null")
	if stored != null:
		_expect(stored.embedding == alpha.embedding,
			"Vector stored embedding mismatch")
		_expect(stored.text_payload == "first",
			"Vector stored text payload mismatch")

	var hits: Array = db.Search(
		PackedFloat32Array([1.0, 0.0, 0.0]), 2)
	_expect(not hits.is_empty(), "Vector Search returned no hits")
	if not hits.is_empty():
		_expect(hits[0].id == "alpha", "Vector nearest hit mismatch")
	_expect(db.ListKeys() == PackedStringArray(["alpha", "beta"]),
		"Vector ListKeys mismatch")
	_expect(db.Close(), "Vector Close failed")
	_expect(db.Destroy(path, 3), "Vector Destroy failed")
	db.free()


func _test_stft() -> void:
	var pcm := PackedFloat32Array()
	pcm.resize(2048)
	for index in pcm.size():
		pcm[index] = sin(float(index) * 0.07)

	var mir := PDJE_MIR.new()
	var results: Array = mir.STFT_PCM_DATA(
		pcm,
		1,
		PDJE_MIR.HANNING,
		6,
		0.5,
		false,
		false,
		false,
		false,
		false,
		false)
	_expect(results.size() == 1, "STFT result channel count mismatch")
	if results.size() == 1:
		_expect(not results[0].real.is_empty(), "STFT real output is empty")
		_expect(not results[0].imag.is_empty(), "STFT imaginary output is empty")
	mir.free()


func _test_beat_this() -> void:
	var ai := PDJE_AI.new()
	var detector = ai.CreateBeatThisDetector(
		"res://Project-DJ-Engine/third_party/onnx_models/beat_this_model_final0.onnx")
	_expect(detector != null, "BeatThis detector initialization failed")
	if detector != null:
		var pcm := PackedFloat32Array()
		pcm.resize(2205)
		for index in pcm.size():
			pcm[index] = sin(float(index) * 0.12)
		var result = detector.DetectPCM(pcm, 1, 22050)
		_expect(result != null, "BeatThis DetectPCM returned null")
		if result != null:
			_expect(typeof(result.beats) == TYPE_PACKED_FLOAT64_ARRAY,
				"BeatThis beats result type mismatch")
			_expect(typeof(result.downbeats) == TYPE_PACKED_FLOAT64_ARRAY,
				"BeatThis downbeats result type mismatch")
	ai.free()
