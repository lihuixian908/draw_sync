//#ifndef SQLITETOOLS_H
//#define SQLITETOOLS_H

//#include <QObject>
//#include <QString>
//#include <QtSql/QSqlDatabase>
//#include <QtSql/QSqlQuery>
//#include <QtSql/QSqlError>
//#include <QCoreApplication>
//#include <QDateTime>
//#include <QVariant>
//#include <QList>
//#include <QFileInfo>
//#include <QDir>

//struct WordInfo {
//    int wordID = -1;
//    QString word;
//    QString difficulty;
//};

//struct PlayerScoreInfo{
//    int roundID = -1;
//    QString playerID;
//    int score = 0;
//    int correctCount = 0;
//    int wrongCount = 0;
//    int rank = -1;
//};

//class DBHandler : public QObject
//{
//    Q_OBJECT

//public:

//    explicit DBHandler(const QString& databasePath, QObject* parent = nullptr);
//    ~DBHandler();

////    int startNewRound(int playerCount);
////    void recordGuess(int roundId, const QString& playerId, const QString& guessedWord, bool isCorrect);

////    void updateDrawerScore(int roundId, const QString& drawerId);
//    void updatePlayerScore(int roundId, const QString& playerId, int scoreToAdd, int correctToAdd, int wrongToAdd);
////    void endRound(int roundId);
////    QList<PlayerScoreInfo> getTotalRanking();

//    QList<PlayerScoreInfo> getRoundRanking(int roundId);

//    bool isValid() const { return m_database.isValid() && m_database.isOpen() && m_wordsDatabase.isValid() && m_wordsDatabase.isOpen(); }

//private:
//    bool initializeGameDatabase();
//    void createTables();
//    bool executeQuery(QSqlQuery& query, const QString& operationName);

//    QSqlDatabase m_database;
//    QSqlDatabase m_wordsDatabase;
//    QString m_databasePath;
//    QString m_wordsDatabasePath;
//};

//#endif // SQLITETOOLS_H














#ifndef SQLITETOOLS_H
#define SQLITETOOLS_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QElapsedTimer>
#include <QRandomGenerator>
#include <QTextStream>

struct PlayerScoreInfo {
    QString playerID;
    int score;
    int correctCount;
    int wrongCount;
    int rank;
};

class DBHandler : public QObject
{
    Q_OBJECT
public:
    explicit DBHandler(const QString& databasePath = "./game_process.db", QObject* parent = nullptr);
    ~DBHandler();

    bool isValid() const { return m_database.isOpen(); }

    void updatePlayerScore(const QString& playerId, int scoreToAdd = 0,
                          int correctToAdd = 0, int wrongToAdd = 0);

    QList<PlayerScoreInfo> getCurrentRanking();

    bool getPlayerScore(const QString& playerId, PlayerScoreInfo& scoreInfo);

    bool resetPlayerScores();
    void insertNewPlayer(const QString& playerId, int score, int correctCount, int wrongCount);

private:
    bool initializeGameDatabase();
    void createTables();
    bool executeQuery(QSqlQuery& query, const QString& operationName);
    void updatePlayerRanks();

    QSqlDatabase m_database;
    QString m_databasePath;
};

#endif // SQLITETOOLS_H
