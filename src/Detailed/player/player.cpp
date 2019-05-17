#include "player.h"

#include "playercontrols.h"
#include <QMediaService>
#include <QMediaPlaylist>
#include <QVideoProbe>
#include <QAudioProbe>
#include <QMediaMetaData>
#include <QtWidgets>
#include <QVideoWidget>
#include <QGraphicsVideoItem>

Player::Player(QWidget *parent)
    :DetailedW(parent)
{
    setSupportedMimeType=std::set<std::string>({

"video/3gpp",
"video/mp2p",
"video/mp2t",
"video/mp4",
"video/mpeg",
"video/quicktime",
"video/x-matroska",
"video/x-ms-asf",
"video/x-msvideo",

"audio/ape",
"audio/mpego",
"audio/mp4",
"video/mp4",
"audio/aac",
"audio/x-karaoke",
"audio/x-wavpack",
"audio/x-musepackc",

"audio/mpeg",
"audio/x-karaoke",

                                               });
//! [create-objs]
    m_player = new QMediaPlayer();
    m_player->setAudioRole(QAudio::VideoRole);
    qInfo() << "Supported audio roles:";
    for (QAudio::Role role : m_player->supportedAudioRoles())
        qInfo() << "    " << role;
    // owned by PlaylistModel
//! [create-objs]

    connect(m_player, &QMediaPlayer::durationChanged, this, &Player::durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &Player::positionChanged);
    connect(m_player, QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &Player::metaDataChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &Player::statusChanged);
    connect(m_player, &QMediaPlayer::bufferStatusChanged, this, &Player::bufferingProgress);
    connect(m_player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &Player::displayErrorMessage);

    //!!FIXME Wneh use QVideoWidget ,it will crash
//    m_videoWidget = new QVideoWidget();


        gv=new QGraphicsView(this);
        auto item=new QGraphicsVideoItem();
        m_player->setVideoOutput(item);
        auto scent=new QGraphicsScene();
        scent->addItem(item);
        gv->setScene(scent);



    m_slider = new QSlider(Qt::Horizontal);
    m_slider->setRange(0, m_player->duration() / 1000);

    m_labelDuration = new QLabel(this);
    connect(m_slider, &QSlider::sliderMoved, this, &Player::seek);

    m_labelHistogram = new QLabel(this);
    m_labelHistogram->setText("Histogram:");
    QHBoxLayout *histogramLayout = new QHBoxLayout;
    histogramLayout->addWidget(m_labelHistogram);




    PlayerControls *controls = new PlayerControls();
    controls->setState(m_player->state());
    controls->setVolume(m_player->volume());
    controls->setMuted(controls->isMuted());

    connect(controls, &PlayerControls::play, m_player, &QMediaPlayer::play);
    connect(controls, &PlayerControls::pause, m_player, &QMediaPlayer::pause);
    connect(controls, &PlayerControls::stop, m_player, &QMediaPlayer::stop);
    connect(controls, &PlayerControls::changeVolume, m_player, &QMediaPlayer::setVolume);
    connect(controls, &PlayerControls::changeMuting, m_player, &QMediaPlayer::setMuted);
    connect(controls, &PlayerControls::changeRate, m_player, &QMediaPlayer::setPlaybackRate);

    connect(m_player, &QMediaPlayer::stateChanged, controls, &PlayerControls::setState);
    connect(m_player, &QMediaPlayer::volumeChanged, controls, &PlayerControls::setVolume);
    connect(m_player, &QMediaPlayer::mutedChanged, controls, &PlayerControls::setMuted);


    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addStretch(1);
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);

    QBoxLayout *layout = new QVBoxLayout;
//    layout->addWidget(m_videoWidget);
    layout->addWidget(gv);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_slider);

    hLayout->addWidget(m_labelDuration);

    layout->addLayout(hLayout);
    layout->addLayout(controlLayout);
    layout->addLayout(histogramLayout);

    setLayout(layout);

    if (!isPlayerAvailable()) {
        QMessageBox::warning(this, tr("Service not available"),
                             tr("The QMediaPlayer object does not have a valid service.\n"\
                                "Please check the media service plugins are installed."));

        controls->setEnabled(false);
    }

//    metaDataChanged();
}

Player::~Player()
{
}

bool Player::isPlayerAvailable() const
{
    return m_player->isAvailable();
}


void Player::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;
    m_slider->setMaximum(m_duration);
}

void Player::positionChanged(qint64 progress)
{
    if (!m_slider->isSliderDown())
        m_slider->setValue(progress / 1000);

    updateDurationInfo(progress / 1000);
}

void Player::metaDataChanged()
{
    if (m_player->isMetaDataAvailable()) {
        setTrackInfo(QString("%1 - %2")
                .arg(m_player->metaData(QMediaMetaData::AlbumArtist).toString())
                .arg(m_player->metaData(QMediaMetaData::Title).toString()));

        if (m_coverLabel) {
            QUrl url = m_player->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();

            m_coverLabel->setPixmap(!url.isEmpty()
                    ? QPixmap(url.toString())
                    : QPixmap());
        }
    }
}

void Player::seek(int seconds)
{
    m_player->setPosition(seconds * 1000);
}

void Player::statusChanged(QMediaPlayer::MediaStatus status)
{
    handleCursor(status);

    // handle status message
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::StalledMedia:
        setStatusInfo(tr("Media Stalled"));
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
        displayErrorMessage();
        break;
    }
}


void Player::handleCursor(QMediaPlayer::MediaStatus status)
{
#ifndef QT_NO_CURSOR
    if (status == QMediaPlayer::LoadingMedia ||
        status == QMediaPlayer::BufferingMedia ||
        status == QMediaPlayer::StalledMedia)
        setCursor(QCursor(Qt::BusyCursor));
    else
        unsetCursor();
#endif
}

void Player::bufferingProgress(int progress)
{
    setStatusInfo(tr("Buffering %4%").arg(progress));
}

void Player::setTrackInfo(const QString &info)
{
    m_trackInfo = info;
    if (!m_statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
    else
        setWindowTitle(m_trackInfo);
}

void Player::setStatusInfo(const QString &info)
{
    m_statusInfo = info;
    if (!m_statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
    else
        setWindowTitle(m_trackInfo);
}

void Player::displayErrorMessage()
{
    setStatusInfo(m_player->errorString());
}

void Player::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
            currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
            m_duration % 60, (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    m_labelDuration->setText(tStr);
}

void Player::showDoc(Rcl::Doc doc)
{
    auto path=
    QUrl(QString::fromStdString(doc.url));//.replace("file://",""));
//    qDebug()<<"url:"<<path;
    QFileInfo f(path.toLocalFile());
    if(f.exists()&&f.isSymLink()){
        qDebug()<<"file is Sym";
        auto fp=f.symLinkTarget();
            path=QUrl::fromLocalFile(fp);
            qDebug()<<"file true path:"<<path;
    }
    m_player->setMedia(path);
    if(m_player->isVideoAvailable()){
    }else{
        this->layout()->replaceWidget(gv,nullptr);
        // audio only
    }
    m_player->play();
    metaDataChanged();
}

bool Player::supportMimeType(std::string &m)
{
    auto supported=QMediaPlayer::hasSupport(QString::fromStdString(m));
    qDebug()<<supported;
    //TODO check?
    return supported>=QMultimedia::MaybeSupported;
}
