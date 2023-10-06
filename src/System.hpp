/*
  Author: [Your Name]
  Date: [Current Date]
  Description: Implementation of the System class for a simulation model.
*/

#ifndef system_h
#define system_h

#include <iostream>
#include <vector>



#include "include/LowerTriangle.hpp"
#include "include/RandomObject.hpp"
#include "include/utils.hpp"
#include "CPI.hpp" //This is the library that calculated the initial agg matrix



class System{
public:
  //hyper paramaters (input):
  int D;
  int N;
  long double s;
  bool INTERNAL;

  //RANDOM OBJECT
  RandomObject *ro;

  //System Paramaters
  long double t; //time
  int Nc;//Number of clusters its good to know

  //System State
  //agents
  std::vector<std::vector<double>> agent_characters;
  //cluster trackets
  std::vector<std::vector<int>> cs;
  std::vector<int> agent_location;

  //Save Last interaction:
  std::pair<int, int> last_link;

  //Aggregation stuff
  long double R;
  LowerTriangle<long double> cp;
  long double normalization_factor;

public:
  System(int D_, int N_, long double s_,bool INTERNAL_,RandomObject &ro_);

  void aggregate(int a1, int a2);
  bool gilStep();


  void printHP();
  void printNC();
  void printCP();


private:
  void initNC();
  void initCP();


};
//-----------------------------------------------------------------------
inline System::System(int D_, int N_, long double s_,bool INTERNAL_,RandomObject &ro_)
  :D(D_),N(N_),s(s_),INTERNAL(INTERNAL_),cp(0),ro(&ro_){

    //initialization
    t =0;
    Nc = N;
    R =  2.0 / (1.0 * (N * (N)));


    //initialize the nodes and clusters
    initNC();

    //initialize the agg matrix
    initCP();   
}
//-----------------------------------------------------------------------
// Aggregate Method Implementation
inline void System::aggregate(int a1, int a2) {
    // Getting the respective clusters
    int c1 = agent_location[a1];
    int c2 = agent_location[a2];

    // Handle internal links
    if (c1 == c2) {
        if (INTERNAL == true) {
            // Add to madeLinks list and update the aggregation matrix
            last_link.first=a1;
            last_link.second =a2;
            cp.set(a1, a2, 0);
        } else {
            throw std::invalid_argument("Internal links not allowed.");
        }
    } else {
        // Add link to madeLinks list
        last_link.first=a1;
        last_link.second =a2;

        Nc--;
        // Update the aggregation matrix
        if (INTERNAL == true) {
            cp.set(a1, a2, 0);
        } else {
            // Remove internal links
            for (int i : cs[c1]) {
                for (int j : cs[c2]) {
                    cp.set(i, j, 0);
                }
            }
        }
        // Update clusters
        for (int j : cs[c2]) {
            cs[c1].push_back(j);
            agent_location[j] = c1;
        }
        cs[c2].clear();
    }
}
//-----------------------------------------------------------------------
inline bool System::gilStep() {

  long double r1 = (long double)(ro->get_double());
  long double  r2 = (long double)(ro->get_double());
  // std::cout << r1 << " " <<r2 <<std::endl; //TODO



  long double alpha = cp.get_cum();
  if (Nc ==1) {
        return false; // If propensities reach zero, end the simulation
  }

  // std::cout << alpha << " " << normalization_factor << " " <<std::endl; //TODO


  //Calculation of Time step //FIX not sure of including the normalization factor here
  
  long double dt = (1.0 / (1.0 * R * normalization_factor)) * std::log(1.0 / (1.0 *r1));
  t += dt;
  // std::cout << R << " " <<dt <<std::endl; //TODO


  //Event Selection + Action
  
  long double  val = r2 * alpha;
  // std::cout << alpha << " " << val << " " <<std::endl; //TODO

  if (val <= alpha){
    int index = cp.search_exceeds_cum(val);
    int row = cp.get_row(index), col = cp.get_col(index);
    if (row >= N || col >= N || row == col) {
        std::cout << alpha<< " " << cp.get_cum() << std::endl;
        std::cout << N<< " " << row << " " << col << std::endl;
        throw std::invalid_argument("Out of bounds aggregation");
    }
    // std::cout << "Aggregating " <<  row  <<"  and " << col << " " <<std::endl; //TODO
    aggregate(row, col);
    return true;
  }


  // return true; //TODO remove this
  throw std::invalid_argument("Shouldn't be here");
  return false;
}

//-----------------------------------------------------------------------
inline void System::initNC(){
    agent_characters.resize(N);
    cs.resize(N);
    agent_location.resize(N);

    //initializing as monomer only with characters in unifrom distribution
    for(int i=0; i<N; i++){
        std::vector<double>  temp_char;
        temp_char.reserve(D); // Reserve space for D elements

        for (int j = 0; j < D; j++) {
            temp_char.push_back(ro->get_double());
        }
        agent_characters[i] = temp_char;

        cs[i].push_back(i);
        agent_location[i] = i;
    }
}

inline void System::initCP(){



  CPI cp_temp = CPI(agent_characters, s);

  cp.reset(cp_temp.lt); //deep copy
  normalization_factor = cp_temp.normalization_factor; //not sure how i will use it yet

}


//-----------------------------------------------------------------------
inline void System::printHP() {
    std::cout << "\nHyperparameters:" << std::endl;
    std::cout << "D: " << D << std::endl;
    std::cout << "N: " << N << std::endl;
    std::cout << "s: " << s << std::endl;
    std::cout << "INTERNAL: " << (INTERNAL ? "true" : "false") << std::endl;
}
inline void System::printNC(){
    std::cout << "\nAGENTS: " << std::endl;
    int i=0;
    for( auto vi: agent_characters){
        std::cout << "\tAgent "<< i << ":\t " << "Character:  " ;
        for( auto vii : vi){
            std::cout << vii << " ";
        }

        std::cout << "\tLocation:  " << agent_location[i];
        std::cout << std::endl;
        i++;
    }
    std::cout << "\nCLUSTERS\n";
    for (int i = 0; i < cs.size(); ++i) {
        if(cs[i].size()>0){
            std::cout << "Cluster " << i << "\t";
            std::cout << "Members: ";
            for (int j : cs[i]) {
                std::cout << j << " ";
            }
            std::cout << "\n";
        }
       
    }
} 
inline void System::printCP(){
  std::cout << "\nCoalesence Probability: " << std::endl;
  cp.print_lower_triangle();
}




//-----------------------------------------------------------------------










#endif //system_h