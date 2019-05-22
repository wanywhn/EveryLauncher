#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QGraphicsView>
#include <QtMultimedia/QMediaPlayer>
#include "../detailedtext.h"

class QAbstractItemView;
class QLabel;
class QMediaPlayer;
class QModelIndex;
class QPushButton;
class QSlider;
class QVideoProbe;
class QVideoWidget;
class QAudioProbe;


class Player : public DetailedW
{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = nullptr);
    ~Player();

    bool isPlayerAvailable() const;


private slots:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void metaDataChanged();


    void seek(int seconds);

    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(int progress);

    void displayErrorMessage();


private:
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void handleCursor(QMediaPlayer::MediaStatus status);
    void updateDurationInfo(qint64 currentInfo);

    QMediaPlayer *m_player = nullptr;
    QVideoWidget *m_videoWidget = nullptr;
    QGraphicsView *gv=nullptr;
    QLabel *m_coverLabel = nullptr;
    QSlider *m_slider = nullptr;
    QLabel *m_labelDuration = nullptr;
    QDialog *m_colorDialog = nullptr;

    QLabel *m_labelHistogram = nullptr;
    QVideoProbe *m_videoProbe = nullptr;
    QAudioProbe *m_audioProbe = nullptr;

    QString m_trackInfo;
    QString m_statusInfo;
    qint64 m_duration;

    // DetailedW interface
public:
    void showDoc() Q_DECL_OVERRIDE;

    // DetailedW interface
public:
    bool supportMimeType(std::string &m) Q_DECL_OVERRIDE;
};

#endif // PLAYER_H
