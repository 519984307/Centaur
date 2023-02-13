create table favorites
(
    id     INTEGER not null
        primary key autoincrement
        unique,
    symbol text    not null,
    plugin text    not null
);

--!!!!
create unique index favorites_id_uindex
    on favorites (id);

--!!!!
create table plugins
(
    uuid         TEXT not null
        primary key
        unique,
    name         TEXT,
    version      TEXT,
    manufacturer TEXT,
    centaur_uuid TEXT
        constraint plugs_ui_versions_uuid_fk
            references ui_versions,
    enabled      INTEGER,
    protected    INTEGER
);

--!!!!
create table ui_versions
(
    uuid           TEXT
        constraint ui_versions_pk
            primary key,
    version_string TEXT
);

--!!!!
create unique index ui_versions_uuid_uindex
    on ui_versions (uuid);

--!!!!
INSERT INTO ui_versions (uuid, version_string)
VALUES ('a15c48b4-460b-4a79-a0a8-8ece90603f85', '0.1.0');

--!!!!