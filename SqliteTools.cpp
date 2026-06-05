//#include <QCoreApplication>
//#include "SqliteTools.h"
//#include <QDebug>

//DBHandler::DBHandler(const QString& databasePath, QObject* parent)
//    : QObject(parent)
//    , m_databasePath(databasePath)
//{
//    m_database = QSqlDatabase::addDatabase("QSQLITE", "game_connection");
//    m_database.setDatabaseName("./game_process.db");

//    if (!initializeGameDatabase()) {
//        qCritical() << "Failed to initialize game database";
//        return;
//    }
//}

//DBHandler::~DBHandler()
//{
//    if (m_database.isOpen()) {
//        m_database.close();
//    }
//    QSqlDatabase::removeDatabase("game_connection");
//}

//bool DBHandler::initializeGameDatabase()
//{
//    QFileInfo fileInfo(m_databasePath);
//    QDir dir = fileInfo.dir();
//    if (!dir.exists()) {
//        if (!dir.mkpath(".")) {
//            qCritical() << "Failed to create database directory:" << dir.path();
//            return false;
//        }
//    }

//    if (!m_database.open()) {
//        qCritical() << "Failed to open game database:" << m_database.lastError();
//        return false;
//    }

//    QSqlQuery query(m_database);
//    query.exec("PRAGMA foreign_keys = ON");
//    query.exec("PRAGMA journal_mode = WAL");

//    if (QFile::exists(m_databasePath)) {
//        qDebug() << "Clearing all existing data from game database";

//        // 关闭外键约束以便快速删除
//        query.exec("PRAGMA foreign_keys = OFF");

//        // 清空所有表（注意顺序以避免外键约束问题）
//        QStringList tables = {"guess_history", "player_scores", "game_rounds"};
//        for (const QString& table : tables) {
//            if (!query.exec("DELETE FROM " + table)) {
//                qWarning() << "Failed to clear table" << table << ":" << query.lastError();
//            } else {
//                qDebug() << "Cleared table:" << table;
//            }
//        }

//        // 重置自增计数器
//        query.exec("DELETE FROM sqlite_sequence");

//        // 重新启用外键约束
//        query.exec("PRAGMA foreign_keys = ON");
//    }

//    createTables();

//    qDebug() << "Game database initialized successfully:" << m_databasePath;
//    return true;
//}

//void DBHandler::createTables()
//{
//    QSqlQuery query(m_database);

//    // game_rounds
//    QString createRoundsTable = R"(
//        CREATE TABLE IF NOT EXISTS game_rounds (
//            round_id INTEGER PRIMARY KEY AUTOINCREMENT,
//            start_time DATETIME DEFAULT CURRENT_TIMESTAMP,
//            end_time DATETIME,
//            player_count INTEGER CHECK (player_count BETWEEN 2 AND 8),
//            status INTEGER DEFAULT 0
//        )
//    )";

//    if (!query.exec(createRoundsTable)) {
//        qCritical() << "Failed to create game_rounds table:" << query.lastError();
//    } else {
//        qDebug() << "game_rounds table created successfully";
//    }

//    // player_scores
//    QString createScoresTable = R"(
//        CREATE TABLE IF NOT EXISTS player_scores (
//            score_id INTEGER PRIMARY KEY AUTOINCREMENT,
//            round_id INTEGER REFERENCES game_rounds(round_id) ON DELETE CASCADE,
//            player_id TEXT NOT NULL,
//            score INTEGER DEFAULT 0,
//            correct_count INTEGER DEFAULT 0,
//            wrong_count INTEGER DEFAULT 0,
//            rank INTEGER CHECK (rank BETWEEN 1 AND 8),
//            UNIQUE(round_id, player_id)
//        )
//    )";

//    if (!query.exec(createScoresTable)) {
//        qCritical() << "Failed to create player_scores table:" << query.lastError();
//    } else {
//        qDebug() << "player_scores table created successfully";
//    }

//    // guess_history
//    QString createHistoryTable = R"(
//        CREATE TABLE IF NOT EXISTS guess_history (
//            guess_id INTEGER PRIMARY KEY AUTOINCREMENT,
//            round_id INTEGER REFERENCES game_rounds(round_id) ON DELETE CASCADE,
//            player_id TEXT NOT NULL,
//            word_guessed TEXT NOT NULL,
//            is_correct INTEGER,
//            guess_time DATETIME DEFAULT CURRENT_TIMESTAMP
//        )
//    )";

//    if (!query.exec(createHistoryTable)) {
//        qCritical() << "Failed to create guess_history table:" << query.lastError();
//    } else {
//        qDebug() << "guess_history table created successfully";
//    }

//    // index
//    QStringList indexes = {
//        "CREATE INDEX IF NOT EXISTS idx_rounds_status ON game_rounds(status)",
//        "CREATE INDEX IF NOT EXISTS idx_scores_round ON player_scores(round_id)",
//        "CREATE INDEX IF NOT EXISTS idx_guess_round_player ON guess_history(round_id, player_id)"
//    };

//    for (const QString& indexSql : indexes) {
//        if (!query.exec(indexSql)) {
//            qWarning() << "Failed to create index:" << query.lastError();
//        }
//    }
//}

//bool DBHandler::executeQuery(QSqlQuery& query, const QString& operationName)
//{
//    if (!query.exec()) {
//        qCritical() << "Failed to" << operationName << ":" << query.lastError();
//        return false;
//    }
//    return true;
//}

////void DBHandler::updateDrawerScore(int roundId, const QString& drawerId)
////{
////    if (!isValid() || roundId <= 0 || drawerId.isEmpty()) {
////        qWarning() << "Invalid parameters for updateDrawerScore - roundId:" << roundId << "drawerId:" << drawerId;
////        return;
////    }

////    QSqlQuery query(m_database);

////    // 检查是否已经存在该画手的记录
////    query.prepare("SELECT score_id FROM player_scores WHERE round_id = ? AND player_id = ?");
////    query.addBindValue(roundId);
////    query.addBindValue(drawerId);

////    if (executeQuery(query, "check drawer score existence")) {
////        if (query.next()) {
////            qDebug() << "Drawer score already exists for round" << roundId << "player" << drawerId;
////            return; // 记录已存在，不需要重复添加
////        }
////    }

////    // 为画手插入初始零成绩记录
////    if (roundId == 1) {
////        // 第一轮：直接插入零成绩
////        query.prepare(R"(
////            INSERT INTO player_scores (round_id, player_id, score, correct_count, wrong_count)
////            VALUES (?, ?, 0, 0, 0)
////        )");
////        query.addBindValue(roundId);
////        query.addBindValue(drawerId);
////    } else {
////        // 第二轮及以后：累积上一轮的成绩
////        query.prepare(R"(
////            INSERT INTO player_scores (round_id, player_id, score, correct_count, wrong_count)
////            SELECT
////                ?, ?,
////                COALESCE((SELECT score FROM player_scores WHERE round_id = ? - 1 AND player_id = ?), 0),
////                COALESCE((SELECT correct_count FROM player_scores WHERE round_id = ? - 1 AND player_id = ?), 0),
////                COALESCE((SELECT wrong_count FROM player_scores WHERE round_id = ? - 1 AND player_id = ?), 0)
////        )");
////        query.addBindValue(roundId);
////        query.addBindValue(drawerId);
////        query.addBindValue(roundId);
////        query.addBindValue(drawerId);
////        query.addBindValue(roundId);
////        query.addBindValue(drawerId);
////        query.addBindValue(roundId);
////        query.addBindValue(drawerId);
////    }

////    if (executeQuery(query, "insert drawer initial score")) {
////        qDebug() << "Successfully added initial score for drawer - round:" << roundId << "player:" << drawerId;
////    } else {
////        qWarning() << "Failed to add initial score for drawer - round:" << roundId << "player:" << drawerId;
////    }
////}

//void DBHandler::updatePlayerScore(int roundId, const QString& playerId, int scoreToAdd, int correctToAdd, int wrongToAdd)
//{
//    if (!isValid()) return;
//    QSqlQuery query(m_database);

//    if (roundId == 1) {
//        query.prepare(R"(
//            INSERT INTO player_scores (round_id, player_id, score, correct_count, wrong_count)
//            VALUES (?, ?, ?, ?, ?)
//            ON CONFLICT(round_id, player_id) DO UPDATE SET
//                score = score + excluded.score,
//                correct_count = correct_count + excluded.correct_count,
//                wrong_count = wrong_count + excluded.wrong_count
//        )");
//    } else {
//        query.prepare(R"(
//            INSERT INTO player_scores (round_id, player_id, score, correct_count, wrong_count)
//            SELECT
//                ?, ?,
//                COALESCE((SELECT score FROM player_scores WHERE round_id = ? - 1 AND player_id = ?), 0) + ?,
//                COALESCE((SELECT correct_count FROM player_scores WHERE round_id = ? - 1 AND player_id = ?), 0) + ?,
//                COALESCE((SELECT wrong_count FROM player_scores WHERE round_id = ? - 1 AND player_id = ?), 0) + ?
//            ON CONFLICT(round_id, player_id) DO UPDATE SET
//                score = COALESCE((SELECT score FROM player_scores WHERE round_id = ? - 1 AND player_id = ?), 0) + excluded.score,
//                correct_count = COALESCE((SELECT correct_count FROM player_scores WHERE round_id = ? - 1 AND player_id = ?), 0) + excluded.correct_count,
//                wrong_count = COALESCE((SELECT wrong_count FROM player_scores WHERE round_id = ? - 1 AND player_id = ?), 0) + excluded.wrong_count
//        )");
//    }

//    if (roundId == 1) {
//        query.addBindValue(roundId);
//        query.addBindValue(playerId);
//        query.addBindValue(scoreToAdd);
//        query.addBindValue(correctToAdd);
//        query.addBindValue(wrongToAdd);
//    } else {
//        query.addBindValue(roundId);
//        query.addBindValue(playerId);
//        query.addBindValue(roundId); query.addBindValue(playerId); query.addBindValue(scoreToAdd);
//        query.addBindValue(roundId); query.addBindValue(playerId); query.addBindValue(correctToAdd);
//        query.addBindValue(roundId); query.addBindValue(playerId); query.addBindValue(wrongToAdd);

//        query.addBindValue(roundId); query.addBindValue(playerId);
//        query.addBindValue(roundId); query.addBindValue(playerId);
//        query.addBindValue(roundId); query.addBindValue(playerId);
//    }

//    executeQuery(query, "update player score");
//}

//QList<PlayerScoreInfo> DBHandler::getRoundRanking(int roundId)
//{
//    QList<PlayerScoreInfo> ranking;
//    if (!isValid()) return ranking;

//    QSqlQuery query(m_database);

//    query.prepare(R"(
//        SELECT
//            score_id, round_id, player_id, score, correct_count, wrong_count, rank
//        FROM player_scores
//        WHERE round_id = ?
//        ORDER BY rank ASC
//    )");
//    query.addBindValue(roundId);

//    if (executeQuery(query, "get round ranking")) {
//        while (query.next()) {
//            PlayerScoreInfo score;
//            score.roundID = query.value("round_id").toInt();
//            score.playerID = query.value("player_id").toString();
//            score.score = query.value("score").toInt();
//            score.correctCount = query.value("correct_count").toInt();
//            score.wrongCount = query.value("wrong_count").toInt();
//            score.rank = query.value("rank").toInt();
//            ranking.append(score);
//        }
//    }
//    return ranking;
//}








#include <QCoreApplication>
#include "SqliteTools.h"
#include <QFileInfo>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DBHandler::DBHandler(const QString& databasePath, QObject* parent)
    : QObject(parent)
    , m_databasePath(databasePath)
{
    m_database = QSqlDatabase::addDatabase("QSQLITE", "game_connection");
    m_database.setDatabaseName("./game_process.db");

    if (!initializeGameDatabase()) {
        qCritical() << "Failed to initialize game database";
        return;
    }
}

DBHandler::~DBHandler()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
    QSqlDatabase::removeDatabase("game_connection");
}

bool DBHandler::initializeGameDatabase()
{
    QFileInfo fileInfo(m_databasePath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qCritical() << "Failed to create database directory:" << dir.path();
            return false;
        }
    }

    if (!m_database.open()) {
        qCritical() << "Failed to open game database:" << m_database.lastError();
        return false;
    }

    QSqlQuery query(m_database);
    query.exec("PRAGMA foreign_keys = ON");
    query.exec("PRAGMA journal_mode = WAL");

    query.exec("PRAGMA foreign_keys = OFF");

    QStringList oldTables = {"guess_history", "player_scores", "game_rounds"};
    for (const QString& table : oldTables) {
        if (!query.exec("DROP TABLE IF EXISTS " + table)) {
            qWarning() << "Failed to drop table" << table << ":" << query.lastError();
        }
    }

    query.exec("DROP INDEX IF EXISTS idx_rounds_status");
    query.exec("DROP INDEX IF EXISTS idx_scores_round");
    query.exec("DROP INDEX IF EXISTS idx_guess_round_player");
    query.exec("DROP INDEX IF EXISTS idx_player_rank");
    query.exec("DROP INDEX IF EXISTS idx_player_score");

    query.exec("PRAGMA foreign_keys = ON");

    createTables();

    qDebug() << "Game database initialized successfully:" << m_databasePath;
    return true;
}

void DBHandler::createTables()
{
    QSqlQuery query(m_database);

    query.exec("DROP TABLE IF EXISTS player_scores");

    QString createScoresTable = R"(
        CREATE TABLE player_scores (
            player_id TEXT PRIMARY KEY,
            score INTEGER DEFAULT 0,
            correct_count INTEGER DEFAULT 0,
            wrong_count INTEGER DEFAULT 0,
            rank INTEGER DEFAULT 0
        )
    )";

    if (!query.exec(createScoresTable)) {
        qCritical() << "Failed to create player_scores table:" << query.lastError();
    } else {
        qDebug() << "player_scores table created successfully";
    }

    QStringList indexes = {
        "CREATE INDEX IF NOT EXISTS idx_player_rank ON player_scores(rank)",
        "CREATE INDEX IF NOT EXISTS idx_player_score ON player_scores(score)"
    };

    for (const QString& indexSql : indexes) {
        if (!query.exec(indexSql)) {
            qWarning() << "Failed to create index:" << query.lastError();
        }
    }
}

bool DBHandler::executeQuery(QSqlQuery& query, const QString& operationName)
{
    if (!query.exec()) {
        qCritical() << "Failed to" << operationName << ":" << query.lastError();
        return false;
    }
    return true;
}

void DBHandler::updatePlayerScore(const QString& playerId, int scoreToAdd, int correctToAdd, int wrongToAdd)
{
    if (!isValid() || playerId.isEmpty()) {
        qWarning() << "Invalid parameters for updatePlayerScore - playerId:" << playerId;
        return;
    }

    QSqlQuery query(m_database);

    query.prepare(R"(
        INSERT OR REPLACE INTO player_scores (player_id, score, correct_count, wrong_count, rank)
        VALUES (
            ?,
            COALESCE((SELECT score FROM player_scores WHERE player_id = ?), 0) + ?,
            COALESCE((SELECT correct_count FROM player_scores WHERE player_id = ?), 0) + ?,
            COALESCE((SELECT wrong_count FROM player_scores WHERE player_id = ?), 0) + ?,
            0
        )
    )");

    query.addBindValue(playerId);
    query.addBindValue(playerId); query.addBindValue(scoreToAdd);
    query.addBindValue(playerId); query.addBindValue(correctToAdd);
    query.addBindValue(playerId); query.addBindValue(wrongToAdd);

    if (!query.exec()) {
        qCritical() << "Failed to update player score for" << playerId << ":" << query.lastError();
    } else {
        qDebug() << "Successfully updated score for player:" << playerId;
    }

    updatePlayerRanks();
}

void DBHandler::insertNewPlayer(const QString& playerId, int score, int correctCount, int wrongCount)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        INSERT OR REPLACE INTO player_scores (player_id, score, correct_count, wrong_count, rank)
        VALUES (?, ?, ?, ?, 0)
    )");
    query.addBindValue(playerId);
    query.addBindValue(score);
    query.addBindValue(correctCount);
    query.addBindValue(wrongCount);

    if (!query.exec()) {
        qCritical() << "Failed to insert new player" << playerId << ":" << query.lastError();
    } else {
        qDebug() << "Successfully inserted new player:" << playerId;
    }
}

void DBHandler::updatePlayerRanks()
{
    if (!isValid()) return;

    QSqlQuery query(m_database);

    query.exec("UPDATE player_scores SET rank = 0");

    QString updateRankSql = R"(
        UPDATE player_scores
        SET rank = (
            SELECT COUNT(*) + 1
            FROM player_scores ps2
            WHERE ps2.score > player_scores.score
               OR (ps2.score = player_scores.score
                   AND ps2.correct_count > player_scores.correct_count)
               OR (ps2.score = player_scores.score
                   AND ps2.correct_count = player_scores.correct_count
                   AND ps2.wrong_count < player_scores.wrong_count)
        )
    )";

    if (!query.exec(updateRankSql)) {
        qWarning() << "Failed to update player ranks:" << query.lastError();
    }
}

QList<PlayerScoreInfo> DBHandler::getCurrentRanking()
{
    QList<PlayerScoreInfo> ranking;
    if (!isValid()) return ranking;

    QSqlQuery query(m_database);

    query.prepare(R"(
        SELECT
            player_id, score, correct_count, wrong_count, rank
        FROM player_scores
        ORDER BY rank ASC, score DESC, correct_count DESC
    )");

    if (executeQuery(query, "get current ranking")) {
        while (query.next()) {
            PlayerScoreInfo score;
            score.playerID = query.value("player_id").toString();
            score.score = query.value("score").toInt();
            score.correctCount = query.value("correct_count").toInt();
            score.wrongCount = query.value("wrong_count").toInt();
            score.rank = query.value("rank").toInt();
            ranking.append(score);
        }
    }
    return ranking;
}

bool DBHandler::resetPlayerScores()
{
    if (!isValid()) return false;

    QSqlQuery query(m_database);

    query.prepare("DELETE FROM player_scores");

    if (executeQuery(query, "reset player scores")) {
        qDebug() << "All player scores have been reset";
        return true;
    }

    return false;
}

bool DBHandler::getPlayerScore(const QString& playerId, PlayerScoreInfo& scoreInfo)
{
    if (!isValid() || playerId.isEmpty()) return false;

    QSqlQuery query(m_database);

    query.prepare(R"(
        SELECT player_id, score, correct_count, wrong_count, rank
        FROM player_scores
        WHERE player_id = ?
    )");
    query.addBindValue(playerId);

    if (executeQuery(query, "get player score")) {
        if (query.next()) {
            scoreInfo.playerID = query.value("player_id").toString();
            scoreInfo.score = query.value("score").toInt();
            scoreInfo.correctCount = query.value("correct_count").toInt();
            scoreInfo.wrongCount = query.value("wrong_count").toInt();
            scoreInfo.rank = query.value("rank").toInt();
            return true;
        }
    }
    return false;
}
