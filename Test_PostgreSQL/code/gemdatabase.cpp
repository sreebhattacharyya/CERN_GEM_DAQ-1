//-----------

#include <iostream>
#include <pqxx/pqxx> 
#include <string>

//------------

using namespace std;
using namespace pqxx;

string DBNAME = "gemdaq";
string USER = "gemlong";
string PASSWORD = "linuxos16";

//--------------

class component_name
{
    public:

        //to be used with original database
        long cernid;
        vector<int> settings[128];

        int ID,SALARY,AGE;
        string NAME,ADDRESS;


    //Display a particular row of the table
    void display_row()
    {
        
        cout << "ID: " << this->ID << endl;
        cout << "NAME: " << this->NAME << endl;
        cout << "AGE: " << this->AGE << endl;
        cout << "SALARY: " << this->SALARY << endl;
        cout << "ADDRESS: " << this->ADDRESS << endl;
        
        /*
        cout << "CERN ID: " << this->cernid << endl;
        cout << "DATA: " << this->settings << endl;
        */
    }

    //Initialize an object with values
    void initialize(int id,string name, int age, int salary,string address)
    {
        this->ID = id;
        this->SALARY = salary;
        this->AGE = age;
        this->NAME = name;
        this->ADDRESS = address;
    }

    //Insert the values in the database from a single object
    void insert_row(connection *C, string table_name)
    {
        work WW(*C);
        string sql = "INSERT INTO COMPANY(ID,NAME,AGE,ADDRESS,SALARY) VALUES ("+ \
                    to_string(this->ID) + ", '"+to_string(this->NAME)+"', "+ \
                    to_string(this->AGE)+", '"+to_string(this->ADDRESS)+"', "+ \
                    to_string(this->SALARY)+");";
        
        WW.exec(sql);
        WW.commit();
        cout << "Values inserted !" << endl;
    }

    //Insert values into the database from a vector of objects
    void insert_data(connection *C, vector<component_name> data)
    {
        int i;
        for (i=0;i<data.size();i++)
        {
            data[i].insert_row(&(*C),"COMPANY");
        }
    }

    //Display results 
    void display_results(vector<component_name> &data)
    {
        int i;
        for (i=0;i<data.size();i++)
        {
            data[i].display_row();
            cout << "------" << endl << endl;
        }
    }

    //Returns a vector of objects from a query response
    vector<component_name> row_to_object(result R)
    {
        vector<component_name> query_results;

        for (auto const &row: R)
        {
            component_name obj;
            
            obj.ID = stoi(row["ID"].c_str());
            obj.AGE = stoi(row["AGE"].c_str());
            obj.NAME = row["NAME"].c_str();
            obj.SALARY = stoi(row["SALARY"].c_str());
            obj.ADDRESS = row["ADDRESS"].c_str();
            

            //obj.cernid = stoi(row["cernid"].c_str());
            //obj.settings = stoi(row["data"].c_str());

            //obj.display_row();
            query_results.push_back(obj);
        }
        //display_results(query_results);
        return query_results;
    }

};

class postgre_database  // : public component_name //-- hiding the imheritange for the moment
{
    public:

    //Returns the query response of a query 
    result display_datanew(connection *C,string table_name)
    {
            work WW(*C);
            string query = "SELECT * FROM " + table_name +";";

             result r = WW.exec(query);
             for (auto const &row: r)
             {
                //cout << row["ID"].c_str() << "   " << row["NAME"].c_str();
                for (auto const &field: row) 
                        std::cout << field.c_str() << '\t';
                std::cout << std::endl;
             }
              cout << "Table displayed successfully" << endl;
              return r;
    }
};

int main()
{
    try
        {
            connection C("dbname = " + DBNAME + " user = "+ USER +" password = "+ PASSWORD +" \
                hostaddr = 127.0.0.1 port = 5432");
            if (C.is_open()) {
                   cout << "Opened database successfully: " << C.dbname() << endl;
             } 
             else {
                     cout << "Can't open database" << endl;
                     return 1;
              }

              postgre_database ob;
              result r;
              //ob.insert_datanew(&C,"COMPANY");
              //r = ob.display_datanew(&C,"COMPANY");

              component_name obj;
              
              //NEW DATA
              obj.initialize(10,"Brett",32,10000,"Australia");
              obj.display_row();

              //INSERT DATA
              obj.insert_row(&C,"COMPANY");
              r = ob.display_datanew(&C,"COMPANY");
              
              vector<component_name> results;
              //RETURNS A VECTOR OF OBJECTS
              results = obj.row_to_object(r);
              obj.display_results(results);

              C.disconnect ();
        }
        catch(const std::exception& e)
        {
            cerr << e.what() << std::endl;
        }

    return 0;
}