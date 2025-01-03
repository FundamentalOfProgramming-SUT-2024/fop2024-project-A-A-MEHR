create table if not exists users
(
    id       serial primary key,
    username varchar(100) not null,
    email    varchar(100) not null,
    password varchar(100) not null
);

CREATE TABLE if not exists game (
game_id SERIAL PRIMARY KEY,
user_id INT NOT NULL,
start_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
score INT,
FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
);


ALTER TABLE users
add total_score int default 0,
add gold int default 0;
