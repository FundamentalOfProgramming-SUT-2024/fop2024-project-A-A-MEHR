#include "libpq-fe.h"
#include "string.h"
#include "ncurses.h"
#include <stdlib.h>

typedef struct {
    int health;
    int hunger;
    int gold;
    int black_gold;
    int Mace;
    int Dagger;
    int Magic_Wand;
    int Normal_Arrow;
    int Sword;
    int Health;
    int Speed;
    int Damage;
    int Normal_Food;
    int Magic_Food;
    int Best_Food;
    int Corrupted_Food;
    int current_gun;
} Game;
typedef struct {
    char username[100];
    char password[100];
    char email[100];
} user;
typedef struct {
    char username[256];
    char email[256];
    int score;
    int gold;
} dto;

typedef struct {
    char username[256];
    int pla_count;
    int score;
    int gold;
    char earliest_game_time[20];
} table_dto;

typedef struct {
    table_dto *users; // Pointer to the array of table_dto structures
    int size;         // Number of elements in the array
} scores_table;
PGconn *conn = NULL; // Global connection object
dto *get_user_with_username(const char *username);

void check_db_error(PGconn *conn, PGresult *res) {
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error executing query: %s\n", PQerrorMessage(conn));
        PQclear(res);
    }
}

int init_db_connection() {
    if (conn != NULL) {
        // Connection already established, no need to reconnect
        return 0;
    }

    // Connection string to connect to PostgreSQL
    const char *conninfo = "dbname=postgres user=postgres password=1821 host=localhost port=5432"; // Modify these as needed

    // Establish the connection to the database
    conn = PQconnectdb(conninfo);

    // Check if the connection was successful
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn); // Clean up if the connection fails
        conn = NULL;
        return 1;
    }

    return 0;
}

int save_user(user user) {
    if (conn == NULL) {
        fprintf(stderr, "Database connection is not established.\n");
        return 1;
    }

    // SQL query to insert a new user
    const char *sql = "INSERT INTO users (username, email, password) VALUES ($1, $2, $3)";

    // Prepare the SQL statement (optional but useful for re-use)
    PGresult *res = PQprepare(conn, "insert_user", sql, 3, NULL);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Preparation failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 1;
    }

    // Values to insert
    const char *values[3];
    values[0] = user.username;
    values[1] = user.email;
    values[2] = user.password;

    // Execute the prepared statement
    res = PQexecPrepared(conn, "insert_user", 3, values, NULL, NULL, 0);
    check_db_error(conn, res); // Check for errors during execution

    // Success message
    printf("User inserted successfully.\n");

    // Clean up
    PQclear(res);
    return 0;
}

void insert_new_game(char *username) {
    const char *find_user_query = "SELECT id FROM users WHERE username = $1";
    PGresult *res = PQexecParams(conn, find_user_query,
                                 1,       // Number of parameters
                                 NULL,    // Parameter types (let libpq infer)
                                 &username,  // Parameter values
                                 NULL,    // Parameter lengths (not needed)
                                 NULL,    // Binary or text parameters (text = NULL)
                                 0);      // Text format

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error finding user: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    // Ensure user exists
    if (PQntuples(res) == 0) {
        fprintf(stderr, "Error: User not found\n");
        PQclear(res);
        return;
    }

    char *user_id = PQgetvalue(res, 0, 0); // Get the user's ID
    PQclear(res);

    // Step 2: Insert a new game for the user
    const char *insert_game_query =
            "INSERT INTO game (user_id, score, start_time) VALUES ($1, 0, CURRENT_TIMESTAMP)";


    const char *params[1] = {user_id}; // User ID and score
    PGresult *insert_res = PQexecParams(conn, insert_game_query,
                                        1,       // Number of parameters
                                        NULL,    // Parameter types
                                        params,  // Parameter values
                                        NULL,    // Parameter lengths
                                        NULL,    // Binary or text parameters
                                        0);      // Text format

    if (PQresultStatus(insert_res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error inserting game: %s\n", PQerrorMessage(conn));
        PQclear(insert_res);
        return;
    }

    printf("Game inserted successfully!\n");
    PQclear(insert_res);
}


// Function to get user from the database by username
int check_username_exists(user user1) {
    // SQL query to fetch a user by username
    const char *sql = "SELECT 1 FROM users WHERE username = $1";

    // Prepare the SQL statement
    PGresult *res = PQprepare(conn, "select_user", sql, 1, NULL);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Preparation failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }

    // Execute the prepared statement with the username as parameter
    const char *paramValues[1];
    paramValues[0] = user1.username;
    res = PQexecPrepared(conn, "select_user", 1, paramValues, NULL, NULL, 0);
//    check_db_error(conn, res); // Check for errors during execution

    // If the query does not return any row
    if (PQntuples(res) == 0) {
        printf("User not found.\n");
        PQclear(res);
        return 0; // User not found
    }
    return 1;
}

// Function to get user by username excluding the password
dto *get_user_with_username(const char *username) {
    if (!conn || PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Invalid database connection.\n");
        return NULL;
    }

    // SQL query selecting all required columns explicitly
    const char *query =
            "SELECT username, email, total_score, gold FROM users WHERE username = $1";

    const char *paramValues[1] = {username};
    PGresult *res = PQexecParams(
            conn,
            query,
            1,            // Number of parameters
            NULL,         // Parameter types (let PostgreSQL infer types)
            paramValues,  // Parameter values
            NULL,         // Parameter lengths (text data)
            NULL,         // Parameter formats (text: 0, binary: 1)
            0             // Result format (text: 0, binary: 1)
    );

    // Check if the query executed successfully
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    // Check if no user was found
    if (PQntuples(res) == 0) {
        printf("User not found.\n");
        PQclear(res);
        return NULL;
    }

    // Dynamically allocate memory for the user structure
    dto *retrieved_user = (dto *) malloc(sizeof(dto));
    if (!retrieved_user) {
        fprintf(stderr, "Memory allocation failed.\n");
        PQclear(res);
        return NULL;
    }

    // Populate the user structure with query results

    snprintf(retrieved_user->username, sizeof(retrieved_user->username), "%s", PQgetvalue(res, 0, 1));
    snprintf(retrieved_user->email, sizeof(retrieved_user->email), "%s", PQgetvalue(res, 0, 2));
    retrieved_user->score = atoi(PQgetvalue(res, 0, 2));
    retrieved_user->gold = atoi(PQgetvalue(res, 0, 3));
    PQclear(res); // Free the PGresult object
    return retrieved_user;
}


void get_password_with_username(user *user1) {
    const char *sql = "SELECT password FROM users WHERE username = $1";

    // Prepare the SQL statement
    PGresult *res = PQprepare(conn, "select_user_pass", sql, 1, NULL);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Preparation failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    // Execute the prepared statement with the username as parameter
    const char *paramValues[1];
    paramValues[0] = user1->username;
    res = PQexecPrepared(conn, "select_user_pass", 1, paramValues, NULL, NULL, 0);
//    check_db_error(conn, res); // Check for errors during execution

    // If the query does not return any row
    if (PQntuples(res) == 0) {
        printf("User not found.\n");
        PQclear(res);
        return; // User not found
    }
    char *password = PQgetvalue(res, 0, 0);
    strncpy(user1->password, password, sizeof(user1->password) - 1);
    user1->password[sizeof(user1->password) - 1] = '\0';
    printw("??%s??", user1->password);

}

int check_username_with_password_exists(user user1) {
    // SQL query to fetch a user by username
    const char *sql = "SELECT 1 FROM users WHERE username = $1 AND password= $2";

    // Prepare the SQL statement
    PGresult *res = PQprepare(conn, "select_user2", sql, 2, NULL);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Preparation failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }

    // Execute the prepared statement with the username as parameter
    const char *paramValues[2];
    paramValues[0] = user1.username;
    paramValues[1] = user1.password;
    res = PQexecPrepared(conn, "select_user2", 2, paramValues, NULL, NULL, 0);
//    check_db_error(conn, res); // Check for errors during execution

    // If the query does not return any row
    if (PQntuples(res) == 0) {
        printf("User not found.\n");
        PQclear(res);
        return 0; // User not found
    }

    // Clean up
    PQclear(res);
    return 1; // Success
}

scores_table get_scores_table() {
    scores_table result = {NULL, 0};

    if (!conn || PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Invalid database connection.\n");
        return result;
    }
//,MIN(game.time) AS earliest_game_time
    const char *query =
            "SELECT COUNT(game.game_id) AS game_count,users.username,users.total_score,users.gold,MIN(game.start_time) AS earliest_game_time FROM users LEFT JOIN game ON users.id = game.user_id GROUP BY users.id ORDER BY users.total_score DESC";

    PGresult *res = PQexecParams(conn, query, 0, NULL, NULL, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return result;
    }

    int num_rows = PQntuples(res);
    if (num_rows == 0) {
        PQclear(res);
        return result;
    }

    table_dto *users = malloc(num_rows * sizeof(table_dto));
    if (!users) {
        fprintf(stderr, "Memory allocation failed.\n");
        PQclear(res);
        return result;
    }

    for (int i = 0; i < num_rows; ++i) {
        snprintf(users[i].username, sizeof(users[i].username), "%s", PQgetvalue(res, i, 1));
        users[i].score = atoi(PQgetvalue(res, i, 2));
        users[i].gold = atoi(PQgetvalue(res, i, 3));
        users[i].pla_count = atoi(PQgetvalue(res, i, 0));
        snprintf(users[i].earliest_game_time, sizeof(users[i].earliest_game_time), "%s", PQgetvalue(res, i, 4));
    }

    PQclear(res);
    result.users = users;
    result.size = num_rows;

    return result;
}

void update_game(Game my_game, char *username, int floor) {

    const char *find_user_query = "SELECT id FROM users WHERE username = $1";
    const char *params1[1] = {username};
    PGresult *res = PQexecParams(conn, find_user_query, 1, NULL, params1, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error finding user: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return;
    }

    if (PQntuples(res) == 0) {
        fprintf(stderr, "Error: User not found\n");
        PQclear(res);
        PQfinish(conn);
        return;
    }

    char *user_id = PQgetvalue(res, 0, 0);
    PQclear(res);

    // Step 2: Retrieve the most recent game ID for the user
    const char *find_game_query = "SELECT game_id FROM game WHERE user_id = $1 ORDER BY start_time DESC LIMIT 1";
    const char *params2[1] = {user_id};
    PGresult *game_res = PQexecParams(conn, find_game_query, 1, NULL, params2, NULL, NULL, 0);

    if (PQresultStatus(game_res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error finding game: %s\n", PQerrorMessage(conn));
        PQclear(game_res);
        PQfinish(conn);
        return;
    }

    if (PQntuples(game_res) == 0) {
        fprintf(stderr, "Error: No game found for user\n");
        PQclear(game_res);
        PQfinish(conn);
        return;
    }

    char *game_id = PQgetvalue(game_res, 0, 0);
    PQclear(game_res);

    // Step 3: Update the game with the values from the Game struct
    const char *update_query =
            "UPDATE game SET "
            "health = health + $1, hunger = hunger + $2, gold = gold + $3, black_gold = black_gold + $4, "
            "Mace = Mace + $5, Dagger = Dagger + $6, Magic_Wand = Magic_Wand + $7, Normal_Arrow = Normal_Arrow + $8, "
            "Sword = Sword + $9, Health_j = Health_j + $10, Speed = Speed + $11, Damage = Damage + $12, "
            "Normal_Food = Normal_Food + $13, Magic_Food = Magic_Food + $14, Best_Food = Best_Food + $15, "
            "Corrupted_Food = Corrupted_Food + $16,floor = $17,score = score + $18 WHERE game_id = $19";

    const char *params3[19];
    char buffer[18][12];
    snprintf(buffer[0], sizeof(buffer[0]), "%d", my_game.health);
    snprintf(buffer[1], sizeof(buffer[1]), "%d", my_game.hunger);
    snprintf(buffer[2], sizeof(buffer[2]), "%d", my_game.gold);
    snprintf(buffer[3], sizeof(buffer[3]), "%d", my_game.black_gold);
    snprintf(buffer[4], sizeof(buffer[4]), "%d", my_game.Mace);
    snprintf(buffer[5], sizeof(buffer[5]), "%d", my_game.Dagger);
    snprintf(buffer[6], sizeof(buffer[6]), "%d", my_game.Magic_Wand);
    snprintf(buffer[7], sizeof(buffer[7]), "%d", my_game.Normal_Arrow);
    snprintf(buffer[8], sizeof(buffer[8]), "%d", my_game.Sword);
    snprintf(buffer[9], sizeof(buffer[9]), "%d", my_game.Health);
    snprintf(buffer[10], sizeof(buffer[10]), "%d", my_game.Speed);
    snprintf(buffer[11], sizeof(buffer[11]), "%d", my_game.Damage);
    snprintf(buffer[12], sizeof(buffer[12]), "%d", my_game.Normal_Food);
    snprintf(buffer[13], sizeof(buffer[13]), "%d", my_game.Magic_Food);
    snprintf(buffer[14], sizeof(buffer[14]), "%d", my_game.Best_Food);
    snprintf(buffer[15], sizeof(buffer[15]), "%d", my_game.Corrupted_Food);
    snprintf(buffer[16], sizeof(buffer[16]), "%d", floor);
    snprintf(buffer[17], sizeof(buffer[17]), "%d", my_game.gold + my_game.black_gold);
    params3[18] = game_id;

    for (int i = 0; i < 18; i++) {
        params3[i] = buffer[i];
    }

    PGresult *update_res = PQexecParams(conn, update_query, 19, NULL, params3, NULL, NULL, 0);

    if (PQresultStatus(update_res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error updating game: %s\n", PQerrorMessage(conn));
    } else {
        printf("Game updated successfully!\n");
    }

    PQclear(update_res);
    //  PQfinish(conn);
}

int get_last_game_floor(const char *username) {


    // Step 1: Retrieve user ID for the username
    const char *find_user_query = "SELECT id FROM users WHERE username = $1";
    const char *params1[1] = {username};
    PGresult *res = PQexecParams(conn, find_user_query, 1, NULL, params1, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error finding user: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return -1; // Error
    }

    if (PQntuples(res) == 0) {
        fprintf(stderr, "Error: User not found\n");
        PQclear(res);
        PQfinish(conn);
        return -1; // User not found
    }

    char *user_id = PQgetvalue(res, 0, 0);
    PQclear(res);

    // Step 2: Retrieve the floor of the most recent game for the user
    const char *find_game_query = "SELECT floor FROM game WHERE user_id = $1 ORDER BY start_time DESC LIMIT 1";
    const char *params2[1] = {user_id};
    PGresult *game_res = PQexecParams(conn, find_game_query, 1, NULL, params2, NULL, NULL, 0);

    if (PQresultStatus(game_res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error finding game: %s\n", PQerrorMessage(conn));
        PQclear(game_res);
        PQfinish(conn);
        return -1; // Error
    }

    if (PQntuples(game_res) == 0) {
        fprintf(stderr, "Error: No game found for user\n");
        PQclear(game_res);
        PQfinish(conn);
        return -1; // No game found
    }

    int floor = atoi(PQgetvalue(game_res, 0, 0)); // Retrieve the floor value
    PQclear(game_res);
//    PQfinish(conn);

    return floor; // Return the floor
}

void update_user_total_score(Game my_game, const char *username) {
    // Step 1: Find user ID
    const char *find_user_query = "SELECT id FROM users WHERE username = $1";
    const char *params1[1] = {username};
    PGresult *res = PQexecParams(conn, find_user_query, 1, NULL, params1, NULL, NULL, 0);

    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error finding user: %s\n", PQerrorMessage(conn));
        if (res) PQclear(res);
        return;
    }

    if (PQntuples(res) == 0) {
        fprintf(stderr, "Error: User not found\n");
        PQclear(res);
        return;
    }

    char *user_id = PQgetvalue(res, 0, 0); // Get user ID
    PQclear(res);

    // Step 2: Update total score
    const char *update_query = "UPDATE users SET total_score = total_score + $1,gold=gold+$2 WHERE id = $3";
    char score_buffer[20];
    char gold[20];
    snprintf(score_buffer, sizeof(score_buffer), "%d", 1 * my_game.gold + 3 * my_game.black_gold);
    const char *params[3] = {score_buffer, gold, user_id};

    PGresult *update_res = PQexecParams(conn, update_query, 3, NULL, params, NULL, NULL, 0);

    if (!update_res || PQresultStatus(update_res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error updating total score: %s\n", PQerrorMessage(conn));
    } else {
        printf("User's total score updated successfully!\n");
    }

    if (update_res) PQclear(update_res);
}


