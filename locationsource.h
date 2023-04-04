#ifndef LOCATIONSOURCE_H
#define LOCATIONSOURCE_H

#include <QObject>

#include <QGeoPositionInfoSource>
#include <QLocale>

#include <memory>

class NetworkManager;

class LocationSource : public QObject {
  Q_OBJECT
  Q_PROPERTY(QGeoCoordinate coordinate READ coordinate NOTIFY coordinateChanged)
  Q_PROPERTY(QString location READ location NOTIFY locationChanged)
 public:
  explicit LocationSource(NetworkManager &networkManager,
                          QObject *parent = nullptr);
  QGeoCoordinate coordinate() const;
  QString location() const;

  void processNewLocale(const QLocale &locale);

 signals:
  void coordinateChanged(const QGeoCoordinate &coordinate);
  void locationChanged();

 private:
  void onNetworkReady();
  void updateLocation(const QGeoPositionInfo &info);
  void processGeoError(QGeoPositionInfoSource::Error error);
  void fetchLocation();
  void onLocationFetched(const QString &location);

  const NetworkManager &mNetworkManager;
  std::unique_ptr<QGeoPositionInfoSource> mGeoPositionSource;
  QGeoCoordinate mGeoCoordinate;
  QString mLocation;
  QLocale mLocale{"en_GB"};
};

#endif  // LOCATIONSOURCE_H
