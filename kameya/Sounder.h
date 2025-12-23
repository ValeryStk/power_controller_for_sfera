#ifndef SOUNDER_H
#define SOUNDER_H

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <memory>
#include <QObject>


class Sounder:public QObject
{
    Q_OBJECT
public:
  Sounder();
  void playSound(const QString &sampleName);
  void muteSoundNotifications(bool isOn);
  void getSoundsList(QStringList &list);
  bool isNotificationsMuted() const;

private:
  bool isNotificationsMuted_;
  void createPlayer();
  int lastSampleIndex_;
  std::unique_ptr <QMediaPlayer>   m_player_;
  std::unique_ptr <QMediaPlaylist> m_playlist_;
  QStringList mySounds_;

private slots:
  void playLastSound();
};

#endif // SOUNDER_H
