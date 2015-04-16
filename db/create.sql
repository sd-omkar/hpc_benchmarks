-- Created by Vertabelo (http://vertabelo.com)
-- Script type: create
-- Scope: [tables, references, sequences, views, procedures]
-- Generated at Tue Mar 03 21:46:52 UTC 2015



-- tables
-- Table: Accelerators
CREATE TABLE Accelerators (
    accl_id integer NOT NULL  PRIMARY KEY AUTOINCREMENT,
    type text NOT NULL,
    accl_name text NOT NULL,
    arch text NOT NULL,
    memory integer NOT NULL,
    cores integer NOT NULL,
    details text NOT NULL
);

-- Table: Hosts
CREATE TABLE Hosts (
    host_id integer NOT NULL  PRIMARY KEY AUTOINCREMENT,
    hostname text NOT NULL
);

-- Table: Perf
CREATE TABLE Perf (
    perf_id integer NOT NULL  PRIMARY KEY AUTOINCREMENT,
    json blob
);

-- Table: Results
CREATE TABLE Results (
    result_id integer NOT NULL  PRIMARY KEY AUTOINCREMENT,
    value_type text NOT NULL,
    value real NOT NULL,
    perf_id integer NOT NULL,
    test_id integer NOT NULL,
    FOREIGN KEY (perf_id) REFERENCES Perf (perf_id),
    FOREIGN KEY (test_id) REFERENCES Tests (test_id)
);

-- Table: Source
CREATE TABLE Sources (
    source_id integer NOT NULL  PRIMARY KEY AUTOINCREMENT,
    URL text NOT NULL,
    commit_hash text NOT NULL
);

-- Table: Tests
CREATE TABLE Tests (
    test_id integer NOT NULL  PRIMARY KEY AUTOINCREMENT,
    date datetime NOT NULL,
    name text NOT NULL,
    input text NOT NULL,
    datatype text NOT NULL,
    dim_x integer NOT NULL,
    dim_y integer NOT NULL,
    NNZ integer NOT NULL,
    host_id integer NOT NULL,
	accl_id integer NOT NULL,
    source_id integer NOT NULL,
    system_id integer NOT NULL,
    FOREIGN KEY (host_id) REFERENCES Hosts (host_id),
	FOREIGN KEY (accl_id) REFERENCES Accelerators (accl_id),
    FOREIGN KEY (source_id) REFERENCES Sources (source_id),
    FOREIGN KEY (system_id) REFERENCES Systems (system_id)
);

-- Table: Systems
CREATE TABLE Systems (
    system_id integer NOT NULL  PRIMARY KEY AUTOINCREMENT,
    OS text NOT NULL,
    compiler text NOT NULL,
    version numeric NOT NULL,
    flags text NOT NULL,
    library text NOT NULL,
    library_ver numeric NOT NULL,
    backend text NOT NULL,
    backend_ver numeric NOT NULL
);

-- View: master
CREATE VIEW master AS
	SELECT *
	FROM tests t, results r, systems s, accelerators a, hosts h
	WHERE t.test_id = r.test_id
		AND t.host_id = h.host_id
		AND t.system_id = s.system_id
		AND t.accl_id = a.accl_id;

-- End of file.

