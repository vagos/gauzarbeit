#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h>
#include <exception>
#include <iostream>
#include <string>

using namespace soci;

using std::cin;
using std::cout;
using std::endl;
using std::string;

template<typename T>
void get_data(const string prompt, T& value) {
    cout << prompt;
    cin >> value;
}

void insert_user(session& sql, 
    const string& first_name) {
    
    // Insert data into users table
    sql << "INSERT INTO users(name) VALUES(:fn)",
        use(first_name, "fn");
    
    cout << "> Successfully inserted user." << endl << endl;
}

void display_users(session& sql) {
    
    // Retrieve all rows from users table
    rowset<row> rs = (sql.prepare << "SELECT * FROM users");

    // Iterate through the result set
    for (rowset<row>::const_iterator it = rs.begin(); it != rs.end(); ++it) {
        const row& r = *it;
        
        std::cout << "ID: " << r.get<int>(0) << endl
            << "First Name: " << r.get<string>(1) << endl;
    }
}

int main() {

    try {

        // Connect to MySQL database
        session sql(mysql, "db=testdb user=root password=exblonger");

        // Get data for new user
        string first_name, last_name, email;

        get_data("> Enter first name: ", first_name);

        // Insert a new row into users table
        insert_user(sql, first_name);

        // Retrieve all rows from users table and output data
        display_users(sql);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << endl; 
    }

    return 0;
}
