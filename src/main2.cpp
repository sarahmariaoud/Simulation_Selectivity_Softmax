#include <iostream>
#include <fstream>  // Include the necessary library for file operations
#include <filesystem>

#if defined(_OPENMP)
   #include <omp.h>
#endif

#include  "include/RandomObject.hpp"
#include "include/utils.hpp"

#include "System.hpp"

//----------------------------------------------
// Global variables for hyperparameters
int N_rels;
int D;
int N;
long double s;
bool INTERNAL;

//direcotry stuff
std::string dir;
std::string data_folder;
std::string time_str;

//----------------------------------------------
void run_sim(int rel);


void set_dirs();
void set_global(int argc, char **argv);


void print_one(int argc, char **argv);
void print_two();
//----------------------------------------------
void dev();


//////////////////////////////////////////////////////////////////////////////////////////////////////
//                          CODE STARTS HERE
//////////////////////////////////////////////////////////////////////////////////////////////////////
// IF NO ARGUMENTS PROVIDED RUNS TEST
// Input should be of the form: (total = 7)
//  ./main.out N_rels INTERNAL D N s  "dir/"
//      -N_rels:                number of realizations [ integer > 0]  
//      -INTERNAL:              wether or not to have internal links [ 0->no , 1 -> yes]
//      -D   (INT)              DIMENSION OF CHAT          
//      -N    (INT)             SIZE OF SYSTEM
//      -s     (double )        selectivity par
//      -"dir/":                directory to put data
//                              if empty then saves in current working directory 
//////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv){

    if(argc==1){
        std::cout << "inside test module" << std::endl;
        dev();
    }
    else{
        print_one(argc,argv);
        set_global(argc,argv);
        set_dirs();
        print_two();
        //running the realizations
        #if defined(_OPENMP)
        std::cout << "USING  [" << omp_get_max_threads() << "] THREADS" << std::endl;
        #pragma omp parallel for
        #endif
        for(int rel=0; rel < N_rels; rel++){
            run_sim(rel);
        }

    }
    std::cout << "--------------------------------------------------------------" << std::endl;
    std::cout << "\t\t PROGRAM END" << std::endl;
    std::cout << "--------------------------------------------------------------" << std::endl;
    return 1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//  THIS IS WHERE THE SIMULATION IS RUNNING
//////////////////////////////////////////////////////////////////////////////////////////////////////
void run_sim(int rel){

    //okay first lets decide whats the data we are gonna write 
    std::string str_nodes = data_folder+"/"+time_str+"-"+std::to_string(rel)+".node.csv";
    std::string str_edges = data_folder+"/"+time_str+"-"+std::to_string(rel)+".edge.csv";
    //making the folders
    std::ofstream node_file(str_nodes);
    std::ofstream edge_file(str_edges);

    // Check if the file is opened successfully
    if (!node_file.is_open()) throw std::invalid_argument("error opening node file");
    if (!edge_file.is_open()) throw std::invalid_argument("error opening edge file");

    node_file << "NodeLabel";
    for(int i=0; i<D; i++) node_file << ",x" << i;
    node_file << std::endl;
    edge_file << "Node1,Node2,Step,Time" << std::endl;

    //initializing the system
    RandomObject ro = RandomObject();
    System sys(D,N,s,INTERNAL,ro);

    // //saving the nodes to a file
    // for(int i=0; i<s.N; i++) node_file << i << "," << s.agent_types[i] << std::endl;
    //saving the nodes to a file
    for(int i=0; i< sys.N; i++){
        node_file << i;
        for(int j =0; j<D; j++) node_file << "," << sys.agent_characters[i][j];
        node_file << std::endl;
    }



    //THIS IS WHERE THE SIMULATION RUNS
    bool cont= true;
    int counter = 0;
    while(cont){
        // std::cout << "STEP: " <<counter << std::endl;
        if(counter!=0) edge_file << sys.last_link.first << "," << sys.last_link.second << "," << counter << "," << sys.t << std::endl;
        
        cont = sys.gilStep();
        counter++;
    }
    // std::cout << "STEP: " <<counter << std::endl;
    edge_file << sys.last_link.first << "," << sys.last_link.second << "," << counter << "," << sys.t << std::endl;
    //closing the folder
    node_file.close();
    edge_file.close();


}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
void set_global(int argc, char **argv){
    //N_rels
    N_rels = std::stoi(argv[1]);
    //Internal
    if(std::stoi(argv[2])==0) INTERNAL = false;
    else if(std::stoi(argv[2])==1) INTERNAL = true;
    D = std::stoi(argv[3]);
    N = std::stoi(argv[4]);
    s = std::stod(argv[5]);

    dir = argv[argc-1];
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
void set_dirs(){
    //CHECKING IF THE DIRECTORY IS PROPER
    if (!((dir[dir.length()-1] == '/' )) && (dir.length()>0)) throw std::invalid_argument("directory is not valid ");
    std::string strInternal = "";
    if(INTERNAL) strInternal = "INTERNAL_";
    data_folder = dir+strInternal+"D_"+tostr(D) +"_N_"+tostr(N) +"_s_"+tostr(s);
    std::filesystem::create_directories(data_folder);
    time_str = get_time_string();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
void print_one(int argc, char **argv){
    //----------
    std::cout << "--------------------------------------------------------------" << std::endl;
    std::cout << "\t\t PROGRAM START" << std::endl;
    std::cout << "--------------------------------------------------------------" << std::endl;
    std::cout << "Number of arguments =  " << argc << std::endl;
    for(int i=0; i<argc; i++){
        std::cout << argv[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "--------------------------------------------------------------" << std::endl;
    int Nargs = 7;
    if(argc!=Nargs) throw std::invalid_argument("wrong number of arguments");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
void print_two(){
    std::cout << "\t\t INPUT ARGUEMTNS" << std::endl;
    std::cout << "--------------------------------------------------------------" << std::endl;
    std::cout << "Number of Realizations: " << N_rels << std::endl;
    std::cout << "Dimension: " << D << std::endl;
    std::cout << "(N): " << N << std::endl;
    std::cout << "(s): " << s << std::endl;
    std::cout << "Internal Links (0:False 1:True): " << INTERNAL << std::endl;

    std::cout << "--------------------------------------------------------------" << std::endl;
    std::cout << "\t\t DIRECTORIES"  << std::endl;
    std::cout << "--------------------------------------------------------------" << std::endl;
    std::cout << "WRITTING DATA TO : " << data_folder<< "/" <<std::endl;
    std::cout << "FILENAMES: " << time_str << std::endl;
    std::cout << "--------------------------------------------------------------" << std::endl;
    std::cout << "\t\t SIMULATION START"  << std::endl;
    std::cout << "--------------------------------------------------------------" << std::endl;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//  TESTING CENTER
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
void dev(){

    N_rels =1;

    //Input
    D= 2;
    N = 10;
    s = 0.8293;
    INTERNAL=false;

    dir = "out/test/";

    set_dirs();
    print_two();

    run_sim(0);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
