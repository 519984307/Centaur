create table log
(
    id      integer not null
        constraint log_pk
            primary key autoincrement,
    date    text,
    session integer,
    user    text,
    level   integer,
    source  text,
    message text
);