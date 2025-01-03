#include "libpq-fe.h"
#include "string.h"
#include "ncurses.h"
#include <stdlib.h>

typedef struct {
    char username[100];
    char password[100];
    char email[100];
} user;

PGconn *conn = NULL; // Global connection object

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
