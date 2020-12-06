#include <iostream>
#include <vector>
#include <random>
#include <tuple>
#include <cmath>
#include <functional>
#include <cfloat>

using namespace std;

double randomDouble(double from, double to) {
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_real_distribution<double> distribution(from, to);
    return distribution(generator);
}

/**
 * Dot on a 2D cartesian plane.
 */
class Dot {

public:
    double x{0.0};
    double y{0.0};

    /**
     * Fitness will represent how big the value of z coordinate is.
     */
    double fitness{0.0};

    Dot(double x, double y) {
        this->x = x;
        this->y = y;
    };

};

std::ostream &operator<<(std::ostream &stream, const Dot dot) {
    return stream << "(x=" << dot.x << ",y=" << dot.y << ",z/fitness=" << dot.fitness << ")";
}

/**
 * Genetic algorithm which tries to find global maximum for any function with two variables.
 */
class GeneticAlgorithm {

public:

    explicit GeneticAlgorithm(int populationSize,
                              const function<double(double, double)> &function,
                              pair<int, int> plotSize) {
        this->size = populationSize;
        this->fun = function;
        this->plotSize = plotSize;
        population.reserve(populationSize);
    }

    Dot run(int times = 300) {
        initialization();
        for (int i = 0; i < times; ++i) {
            calculateFitness();
            naturalSelection();
            mutate();
        }

        alpha.fitness = originalFitness(alpha.fitness);

        return alpha;
    }

private:
    int size;
    vector<Dot> population;
    function<double(double, double)> fun;

    double fitnessSum{0};

    pair<int, int> plotSize;
    Dot alpha = Dot{0, 0};

    /**
     * Used for transforming dots fitness back to z coordinate.
     */
    double minFitness{DBL_MAX};

    void initialization() {
        for (int i = 0; i < size; ++i) {
            double x = randomDouble(plotSize.first, plotSize.second);
            double y = randomDouble(plotSize.first, plotSize.second);
            Dot dot = Dot{x, y};
            population.push_back(dot);
        }
    }

    void calculateFitness() {
        for (auto &dot : population) {
            dot.fitness = fun(dot.x, dot.y);

            if (dot.fitness < minFitness) {
                minFitness = dot.fitness;
            }
        }

        normaliseFitness();
    }

    /**
     * Normalise (if necessary) to non-negative numbers for roulette wheel selection.
     */
    void normaliseFitness() {
        auto maxFitness = DBL_MIN;
        for (auto &dot : population) {
            dot.fitness += abs(minFitness);

            // find normalised alpha
            if (dot.fitness > maxFitness) {
                maxFitness = dot.fitness;
                alpha = dot;
            }
        }
    }

    void calculateFitnessSum() {
        fitnessSum = 0;
        for (auto &dot : population) {
            fitnessSum += dot.fitness;
        }
    }

    /**
     * Roulette wheel selection of parent.
     */
    Dot selectParent() {
        double rand = randomDouble(0, fitnessSum);
        double offset = 0.0;
        for (auto &dot : population) {
            offset += dot.fitness;
            if (offset > rand) {
                return dot;
            }
        }

        //never happens
        cerr << "Roulette wheel failed." << endl;
        exit(1);
    }

    /**
     * Crates new generation containing an alpha dot and children of recombined parents chosen by
     * the roulette wheel selection.
     */
    void naturalSelection() {
        vector<Dot> newGeneration;
        newGeneration.reserve(population.size());

        calculateFitnessSum();
        newGeneration.push_back(alpha);

        for (int i = 0; i < population.size() - 1; ++i) {
            Dot parent1 = selectParent();
            Dot parent2 = selectParent();
            newGeneration.push_back(recombine(parent1, parent2));
        }

        population.clear();
        population.insert(population.begin(), newGeneration.begin(), newGeneration.end());
    }

    /**
     * This seems to return best results. Returns dot with average x and average y of its parents.
     */
    static Dot recombine(Dot parent1, Dot parent2) {
        return Dot{
                (parent1.x + parent2.x) / 2,
                (parent1.y + parent2.y) / 2
        };
    }

    void mutate() {
        double rate = 0.08;
        for (auto dot : population) {
            double rand = randomDouble(0.0, 1.0);
            if (rand < rate) {
                dot.x += randomDouble(-0.1, 0.1);
                dot.y += randomDouble(-0.1, 0.1);
            }
        }
    }

    /**
     * Transform fitness back into a z coordinate.
     */
    double originalFitness(double fitness) const {
        return fitness - abs(minFitness);
    }

};

// function to find the global maximum of
double labFunction(double x, double y) {
    double numerator = 7. * x * y;
    double denominator = exp(pow(x, 2) + pow(y, 2) + x);
    return numerator / denominator;
}

double optimize() {
    GeneticAlgorithm ga = GeneticAlgorithm(100, labFunction, pair(-5, 5));
    return ga.run().fitness;
}

int main() {
    for (int i = 0; i < 10; ++i) {
        GeneticAlgorithm ga = GeneticAlgorithm(200, labFunction, pair(-5, 5));
        cout << ga.run() << endl;
    }
}

