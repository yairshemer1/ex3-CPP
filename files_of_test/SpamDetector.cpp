#include <iostream>
#include <fstream>
#include "HashMap.hpp"

#define INVALID_USAGE std::cerr << "Usage: SpamDetector <database path> <message path> <threshold>\n"; return EXIT_FAILURE;
#define INVALID_INPUT std::cerr << "Invalid input\n"; return EXIT_FAILURE;

static const char SEP_DELIM = ',';

static const int NUM_OF_ARGS = 4;

static const int DB_LOC = 1;

static const int MSG_LOC = 2;

static const int TH_LOC = 3;

static const char *const SPAM_MSG = "SPAM\n";

static const char *const NOT_SPAM_MSG = "NOT_SPAM\n";

/**
 * A function to parse the database into a phrase and a number and insert it to the map
 * @param in stream
 * @param map hash map
 * @return true if the parse succeeded, false otherwise
 */
bool loadDataBase(std::ifstream &in, HashMap<std::string, int> *map)
{
    std::string phrase, number;
    int value;
    while (getline(in, phrase, SEP_DELIM))
    {
        getline(in, number, '\n');
        if (number.empty() || phrase.empty() || phrase.find('\n') != std::string::npos)
        {
            return false;
        }
        if (number.find('\r') != std::string::npos)
        {
            number = number.substr(0, number.size() - 1);
        }
        value = std::stoi(number);
        if (std::to_string(value) != number || value < 0)
        {
            return false;
        }
        map->insert(phrase, value);
    }
    return true;
}

/**
 * A function to make a string lower case
 * @param str to lower case
 * @return the lower cased string
 */
std::string &toLowerCase(std::string &str)
{
    for (auto &s : str)
    {
        s = tolower(s);
    }
    return str;
}

/**
 * A function to analyze a database and message and report if a spam is detected
 * @param argc number of arguments
 * @param argv array of string arguments
 * @return EXIT_SUCCESS upon success, EXIT_FAILURE otherwise
 */
int main(int argc, char *argv[])
{
    std::string line;
    int sum = 0, threshold;
    HashMap<std::string, int> map;
    if (argc != NUM_OF_ARGS)
    {
        INVALID_USAGE
    }
    std::ifstream database(argv[DB_LOC]);
    std::ifstream message(argv[MSG_LOC]);
    try
    {
        threshold = std::stoi(argv[TH_LOC]);
    }
    catch (const std::invalid_argument &)
    {
        database.close();
        message.close();
        INVALID_INPUT
    }
    if (std::to_string(threshold) != argv[TH_LOC] || threshold <= 0 || !database || !message ||
        !loadDataBase(database, &map))
    {
        database.close();
        message.close();
        INVALID_INPUT
    }
    while (getline(message, line))
    {
        line = toLowerCase(line);
        for (const std::pair<std::string, int> &p: map)
        {
            std::string key = p.first;
            std::string lowerCaseKey = toLowerCase(key);
            for (size_t loc = line.find(lowerCaseKey);
                 loc != std::string::npos; (loc = line.find(lowerCaseKey, loc + 1)))
            {
                sum += p.second;
            }
        }
    }
    if (sum >= threshold)
    {
        std::cout << SPAM_MSG;
    }
    else
    {
        std::cout << NOT_SPAM_MSG;
    }
    database.close();
    message.close();
    return EXIT_SUCCESS;
}