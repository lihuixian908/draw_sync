#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QKeyEvent>

class GuessInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GuessInputDialog(int wordLength, QWidget *parent = nullptr);
    void setRemainingTime(int seconds);
    QString getGuess() const;
    void clearInput();

signals:
    void guessSubmitted(const QString& guess);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onTextChanged(const QString &text);
    void onSubmitClicked();

private:
    void updateDisplay();

    QLabel *m_timeLabel;
    QLabel *m_displayLabel;
    QLineEdit *m_hiddenInput;
    QPushButton *m_submitButton;
    int m_wordLength;
};

class GameTimer : public QObject
{
    Q_OBJECT

public:
    explicit GameTimer(QObject *parent = nullptr);
    void startGuessGame(int durationSeconds, const QString& targetWord, QWidget *parentWidget = nullptr, bool isDrawer = false);
    void startTimer(int durationSeconds, bool isDrawer = false); // just calculate by time withoust window
    void stopTimer(); // stop timer

signals:
    void timeUp();
    // signal for timing up
    void wordGuessed();
    // signal for correct answer
    void guessSubmitted(const QString& guess);

private slots:
    void onTimerTimeout();
    void showInputDialog();

private:
    QTimer *m_timer;
    QTime m_remainingTime;
    QString m_targetWord;
    int m_totalDuration;
    bool m_gameActive;
    //avoid pop-up window repeatly
    bool m_timeoutPopupShown;
    QString m_lastGuess;
    GuessInputDialog *m_inputDialog;
    //save the pointers of parentwidget
    QWidget *m_parentWidget;
    bool m_isDrawer;
};

#endif // GAMETIMER_H
