#include <bits/stdc++.h>

using namespace std;

const int BASE_MAXN = 80000;
const int TEST_MAXN = 20000;
const int USERS = 943;
const int ITEMS = 1682;
const double BETA = 0.5;



int database[1000][2000];


// This function initializes the training database
void init() {
    // open the database and initialize the ratings to 0
    freopen("u1_base.txt", "r", stdin);
    memset(database, 0, sizeof(database));

    // Take input from the base file
    // Input format is mentioned below
    // UserID | ItemID | Rating | Timestamp
    for (int i = 1; i <= BASE_MAXN; ++i) {
        int userId, itemId, rating, timeStamp;
        cin >> userId >> itemId >> rating >> timeStamp;
        database[userId][itemId] = rating;
    }
}

// This function calculates the user mean for all the users
// in the training data
void calcUserMean(map<int, double> &userMean) {
    
    for (int user = 1; user <= USERS; ++user) {
        int total = 0, count = 0;
        for (int item = 1; item <= ITEMS; ++item) {
            if (database[user][item]) {
                total += database[user][item];
                count++;
            }
        }
        if (count) {
            userMean[user] = total / (double) count;
        }
    }
}

// This function calculates the item mean for all the items
// in the training data
void calcItemMean(map<int, double> &itemMean) {

    for (int item = 1; item <= ITEMS; ++item) {
        int total = 0, count = 0;
        for (int users = 1; users <= USERS; ++users) {
            if (database[users][item]) {
                total += database[users][item];
                count++;
            }
        }
        if (count) {
            itemMean[item] = total / (double) count;
        }
    }
}

// This function calculates the user tendency for all the users
// in the training data
void calcUserTendency(map<int, double> &userTendency, map<int, double> &itemMean) {

    for (int user = 1; user <= USERS; ++user) {
        double tendency = 0.0;
        int countItems = 0;

        for (int item = 1; item <= ITEMS; ++item) {
            if (database[user][item]) {
                tendency += (database[user][item] - itemMean[item]);
                countItems++;
            }
        }
        userTendency[user] = tendency / countItems;
    }
}

// This function calculates the item tendency for all the items
// in the training data
void calcItemTendency(map<int, double> &itemTendency, map<int, double> &userMean) {

    for (int item = 1; item <= ITEMS; ++item) {
        double tendency = 0.0;
        int countUsers = 0;

        for (int user = 1; user <= USERS; ++user) {
            if (database[user][item]) {
                tendency += (database[user][item] - userMean[user]);
                countUsers++;
            }
        }
        itemTendency[item] = tendency / countUsers;
    }
}

// This function calculates
// MAE,
// RMSE,
// Precision,
// Recall,
// F1-measure
void evaluate(map<int, double> &userMean, map<int, double> &userTendency, map<int, double> &itemMean,
              map<int, double> &itemTendency) {
    freopen("u1_test.txt", "r", stdin);

    double MAE = 0.0, RMSE = 0.0, precision, recall, f1Measure;
    int countOfRecommended = 0, countOfRelevant = 0, countOfIntersection = 0;
    int count = 0;

    for (int i = 1; i <= TEST_MAXN; ++i) {
        int userId, itemId, rating, timeStamp;
        cin >> userId >> itemId >> rating >> timeStamp;

        double tendencyUser = userTendency[userId];
        double tendencyItem = itemTendency[itemId];

        double predictedRating;


        if (tendencyItem >= 0.0 and tendencyUser >= 0.0) {
            predictedRating = max(tendencyItem + userMean[userId], tendencyUser + itemMean[itemId]);
        } else if (tendencyItem < 0 and tendencyUser < 0) {
            predictedRating = min(tendencyItem + userMean[userId], tendencyUser + itemMean[itemId]);
        } else if (tendencyItem < 0 and tendencyUser >= 0 and (itemMean[itemId] >= userMean[userId])) {
            double temp = BETA * (tendencyItem + userMean[userId]) + (1 - BETA) * (tendencyUser + itemMean[itemId]);
            predictedRating = min(max(userMean[userId], temp), itemMean[itemId]);
        } else if (tendencyUser >= 0 and tendencyItem < 0 and (itemMean[itemId] < userMean[userId])) {
            double temp = BETA * (tendencyItem + userMean[userId]) + (1 - BETA) * (tendencyUser + itemMean[itemId]);
            predictedRating = min(max(itemMean[itemId], temp), userMean[userId]);
        } else {
            predictedRating = userMean[userId] * BETA + (1 - BETA) * itemMean[itemId];
        }

        if (predictedRating >= 3.0 and rating >= 3.0) {
            countOfIntersection++;
            countOfRelevant++;
            countOfRecommended++;
        } else if (predictedRating >= 3.0 and rating < 3.0) {
            countOfRecommended++;
        } else if (predictedRating < 3.0 and rating >= 3.0) {
            countOfRelevant++;
        }

        MAE  += abs(rating - predictedRating);
        RMSE += (rating - predictedRating) * (rating - predictedRating);
        count++;

        //cout << "Actual Rating: " << rating << " Predicted Rating: " << predictedRating << "\n";
    }

    MAE  /= count;
    RMSE /= count;
    RMSE = sqrt(RMSE);

    precision = countOfIntersection / (double) countOfRecommended;
    recall    = countOfIntersection / (double) countOfRelevant;
    f1Measure = (2.0 * precision * recall) / (precision + recall);

    cout << "MAE: " << MAE << "\n";
    cout << "RMSE: " << RMSE << "\n";
    cout << "Precision: " << precision << "\n";
    cout << "Recall: " << recall << "\n";
    cout << "F1-Measure: " << f1Measure << "\n";
}

int main() {

    double startt = clock();
    map<int, double> userMean, userTendency;
    map<int, double> itemMean, itemTendency;

    //initialize the database
    init();

    //calculate the user mean
    calcUserMean(userMean);

    // calculate the item mean
    calcItemMean(itemMean);

    //calculate the user tendency
    calcUserTendency(userTendency, itemMean);

    // calculate the item tendency
    calcItemTendency(itemTendency, userMean);

    evaluate(userMean, userTendency, itemMean, itemTendency);

    cout << "Time taken: " << (clock() - startt) / CLOCKS_PER_SEC << "s\n";
    return 0;
}