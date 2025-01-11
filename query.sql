create table if not exists users
(
    id       serial primary key,
    username varchar(100) not null,
    email    varchar(100) not null,
    password varchar(100) not null
);

CREATE TABLE if not exists game
(
    game_id    SERIAL PRIMARY KEY,
    user_id    INT       NOT NULL,
    start_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    score      INT,
    FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE CASCADE
);


ALTER TABLE users
    add total_score int default 0,
    add gold        int default 0;

ALTER TABLE game
    ADD COLUMN health INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN hunger INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN gold INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN black_gold INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN Mace INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN Dagger INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN Magic_Wand INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN Normal_Arrow INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN Sword INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN Health_j INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN Speed INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN Damage INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN Normal_Food INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN Magic_Food INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN Best_Food INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN Corrupted_Food INT DEFAULT 0 NOT NULL;
ALTER TABLE game
    ADD COLUMN floor INT DEFAULT 1 NOT NULL;
