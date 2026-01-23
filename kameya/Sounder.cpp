#include "Sounder.h"

#include "QDebug"
#include "QDir"

Sounder::Sounder() : isNotificationsMuted_(false) { createPlayer(); }

void Sounder::playSound(const QString &sampleName) {
    if (isNotificationsMuted_) return;
    uint16_t index = 0;
    index = std::distance(
        mySounds_.begin(),
        std::find(mySounds_.begin(), mySounds_.end(), sampleName));
    m_playlist_->setCurrentIndex(index);
    m_player_->play();
    lastSampleIndex_ = index;
}

void Sounder::playLastSound() {
    if (isNotificationsMuted_) return;
    m_playlist_->setCurrentIndex(lastSampleIndex_);
    m_player_->play();
}

void Sounder::muteSoundNotifications(bool isOn) {
    isNotificationsMuted_ = isOn;
}

void Sounder::getSoundsList(QStringList &list) { list = mySounds_; }

bool Sounder::isNotificationsMuted() const { return isNotificationsMuted_; }

void Sounder::createPlayer() {
    isNotificationsMuted_ = false;
    m_player_ = std::make_unique<QMediaPlayer>();
    m_playlist_ = std::make_unique<QMediaPlaylist>(m_player_.get());
    m_player_->setPlaylist(m_playlist_.get());
    m_player_->setVolume(70);
    m_playlist_->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);

    QDir dir(":/sounds");
    QString urlPrefix = QString(":/sounds");
    urlPrefix.prepend("qrc");
    urlPrefix.append("/");

    mySounds_ = dir.entryList();
    for (int i = 0; i < mySounds_.count(); ++i) {
        QString url = urlPrefix + mySounds_.at(i);
        m_playlist_->addMedia(QUrl(url));
    }
}
