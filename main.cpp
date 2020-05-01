/**
 * This is an implementation of Simulated Annealing(SA) algorithm for solving a sudoku puzzle.
 * 
 * Sources:
 * [1]The Theory and Practive of Simulated Annealing by Darrall Henderson, Sheldon H. Jacobson and Alan W. Johnson
 * 
 * @author Sukrucan Taylan ISIKOGLU, 195105106
 */ 
#include <cstdlib>
#include <ctime>
#include <math.h> 
#include <array>
#include <iostream>
#include <unordered_map>
#include <algorithm>

//Algorithm parameters
double tk = 1000;
const int Mk = 10;
const double coolingRate = 0.95;
const double minimumTemperature = 0.3;

/**
 * This class represents a PossibleSolution in solution space.
 */ 
struct PossibleSolution {
    //Holds the sudoku play area
    std::array<std::array<int, 9>, 9> playArea;
    //Holds wheter corresponding position in playArea is free to be changed or not
    //Initialized with correct values after initializing playArea
    std::array<std::array<bool, 9>, 9> isFreeDigit {{
        {false, false, false, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, false}
    }};

    /**
     * Creates a possible solution by placing random numbers such that each row has numbers from 1 to 9.
     * It sets empty positions as free digits so that they can be swapped later when generating a neighboring solution.
     */ 
    PossibleSolution(std::array<std::array<int, 9>, 9>& initialPlayArea) : playArea(initialPlayArea) {
        int randNum;
        for (int i = 0; i < playArea.size(); i++) {
            for (int j = 0; j < playArea[i].size(); j++) {
                if (playArea[i][j] == 0) {
                    do {
                        randNum = rand()%9 + 1;
                    }
                    while (std::find(playArea[i].begin(), playArea[i].end(), randNum) != playArea[i].end());
                    
                    playArea[i][j] = randNum;
                    isFreeDigit[i][j] = true;
                }
            }
        }
    }

    //Copy constructor
    PossibleSolution(const PossibleSolution& rhs) {
        playArea = rhs.playArea;
        isFreeDigit = rhs.isFreeDigit;
    }

    /**
     * Formats and sends the sudoku play board to output stream, used for debugging.
     */ 
    friend std::ostream& operator<<(std::ostream& os, const PossibleSolution& soln) {
            for (int i = 0; i < soln.playArea.size(); i++) {
                if (i%3 == 0 && i != 0) {
                    os << std::endl << "---------------------------------" << std::endl;
                }
                else {
                    os << std::endl;
                }
                
                for (int j = 0; j < soln.playArea[i].size(); j++) {
                    if (j%3 == 0 && j != 0) {
                        os << " | ";
                    }
                    os << " " << soln.playArea[i][j] << " ";
                }
            }
        return os;
    }
};

/**
 * Generates a neighboring solution from current solution by randomly selecting a row and 
 * swapping two columns that are free digits.
 */ 
PossibleSolution generateNeighboringSolution(const PossibleSolution& currentSoln);

/**
 * Returns the number of duplicate elements inside the solution; for each row, column and block.
 * If there are no duplicate elements then it should be the actual solution.
 */ 
int calculateObjectiveFunction(const PossibleSolution& w);

int main() {
    srand(time(0));

    std::array<std::array<int, 9>, 9> playArea {{
        {0, 0, 6, 0, 0, 0, 0, 0, 0},
        {0, 8, 0, 0, 5, 4, 2, 0, 0},
        {0, 4, 0, 0, 9, 0, 0, 7, 0},
        {0, 0, 7, 9, 0, 0, 3, 0, 0},
        {0, 0, 0, 0, 8, 0, 4, 0, 0},
        {6, 0, 0, 0, 0, 0, 1, 0, 0},
        {2, 0, 3, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 5, 0, 0, 0, 4, 0},
        {0, 0, 8, 3, 0, 0, 5, 0, 2}
    }};

    int k = 0;

    PossibleSolution w(playArea);
    std::cout << w << std::endl;

    bool isValid = false;
    while (!isValid) {
        for (int  m = 0; m < Mk; m++) {
            PossibleSolution w_p = generateNeighboringSolution(w);
            std::cout << "New possible solution: " << std::endl;
            std::cout << w_p << std::endl;

            int score = calculateObjectiveFunction(w);
            int score_p = calculateObjectiveFunction(w_p);
            std::cout << "Score of w is " << score << " and score of w' is " << score_p << std::endl;
            
            //If score is 0, this means there are no duplicates in the playArea, which means we found a solution
            if (score == 0) {
                isValid = true;
                break;
            }
            else {
                int delta = score_p - score;
                //If delta is less than or equal to 0 then we have a better solution in w_p, because its score is less than 
                //the solution before
                if (delta <= 0)  { 
                    w = w_p;
                }
                //If not, then we might choose this solution by using the following probability
                else if (rand()%100 < exp(-1*delta / tk)*100) {
                    w = w_p;
                }
                std::cout << "****************************************" << std::endl;
            }
        }
        k++;
        if (tk > minimumTemperature) {
            std::cout << "New temperature is " << tk << std::endl;
            tk = tk*coolingRate;
        }
    }

    std::cout << "We found a solution!" << std::endl;
    std::cout << w << std::endl;
    std::cout << "It took " << k << " iterations." << std::endl;
}

PossibleSolution generateNeighboringSolution(const PossibleSolution& currentSoln) {
    PossibleSolution tmp(currentSoln);
    int i = rand() % 9;
    int j1 = rand() % 9;
    int j2 = rand() % 9;
    //Select a free digit in this row
    while (!tmp.isFreeDigit[i][j1]) {
        j1 = rand() % 9;
    }
    while (!tmp.isFreeDigit[i][j2]) {
        j2 = rand() % 9;
    }
    std::swap(tmp.playArea[i][j1], tmp.playArea[i][j2]);
    std::cout << "Changing row: " << i << " and swapping columns: " << j1 << " and " << j2 << std::endl; 
    return tmp;
}

int calculateObjectiveFunction(const PossibleSolution& w) {
    int score = 0;

    //Calculate duplicates in rows
    for (std::array<int,9> row : w.playArea) {
        std::unordered_map<int, int> tmp {
            {1, 0}, {2, 0}, {3, 0},
            {4, 0}, {5, 0}, {6, 0},
            {7, 0}, {8, 0}, {9, 0}
        };
        //count each element
        for (int i = 0; i < row.size(); i++) {
            tmp[row[i]]++;
        }

        //for each element only add it to score if it is greater than one(it is a duplicate)
        //then subtract one to get number of extra duplicates
        for (auto p : tmp) {
            if (p.second > 1) {
                score += p.second - 1;
            }
        }
    }
    //Calculate duplicates in columns
    //for each column
    for (int j = 0; j < w.playArea[0].size(); j++) {
        std::unordered_map<int, int> tmp {
            {1, 0}, {2, 0}, {3, 0},
            {4, 0}, {5, 0}, {6, 0},
            {7, 0}, {8, 0}, {9, 0}
        };
        //for each row count each element
        for (int i = 0; i < w.playArea.size(); i++) {
            tmp[w.playArea[i][j]]++;
        }

        //for each element only add it to score if it is greater than one(it is a duplicate)
        //then subtract one to get number of extra duplicates
        for (auto p : tmp) {
            if (p.second > 1) {
                score += p.second - 1;
            }
        }
    }
    //Calculate duplicates in blocks of 3x3
    //for each block
    /*
     * We enumarate blocks as follows:
     *    
     * 0 1 2
     * 3 4 5
     * 6 7 8
     * 
     */ 
    for (int block = 0; block < 9; block++) {
        int startingRow = 0;
        //get the row number each block starts before looping inside the block
        switch (block) {
            case 0: case 1: case 2:
                startingRow = 0;
                break;
            case 3: case 4: case 5:
                startingRow = 3;
                break;
            case 6: case 7: case 8:
                startingRow = 6;
                break;
        }

        std::unordered_map<int, int> tmp {
                {1, 0}, {2, 0}, {3, 0},
                {4, 0}, {5, 0}, {6, 0},
                {7, 0}, {8, 0}, {9, 0}
        };

        for (int i = startingRow; i < startingRow + 3; i++) {
            for (int j = (block%3)*3; j < (block%3)*3 + 3; j++) {
                tmp[w.playArea[i][j]]++;
            }      
        }

        //for each element only add it to score if it is greater than one(it is a duplicate)
        //then subtract one to get number of extra duplicates
        for (auto p : tmp) {
            if (p.second > 1) {
                score += p.second - 1;
            }
        }
    }
    return score;
}