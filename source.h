#ifndef DFXML_SOURCE_TAG_H
#define DFXML_SOURCE_TAG_H

#include <QDateTime>
#include <QXmlStreamReader>
#include <QList>
#include <QString>

#include "byterun.h"
#include "volume.h"

class DfxmlSource
{
	public:
		DfxmlSource();

		void loadFromXmlStream(QXmlStreamReader &stream);
		void save(QXmlStreamWriter &stream);

		void setImageFileName(const QString &name);
		void setImageSize(const QString &size);
		void setSectorSize(const QString &size);
		void setDeviceSectors(const QString &sectors);
		void setDeviceModel(const QString &model);
		void setDeviceSerialNumber(const QString &sn);
		void setAcquisitionDateTime(const QDateTime &dt);
		void setTextNote(const QString &note);
		void setHashDigest(const QString &type, const QString &hashdigest);

		QString imageName() const;
		quint64 imageSize() const;
		quint64 sectorSize() const;
		quint64 deviceSectors() const;
		QString deviceModel() const;
		QString deviceSerialNumber() const;
		QDateTime acquisitionDateTime() const;
		QString textNote() const;
		QString hashDigest(const QString &type) const;

#ifndef QT_NO_DEBUG_OUTPUT
		void debugDump() const;
#endif

	private:
		void clearData();

		QString m_imageName;
		quint64 m_imageSize;
		quint64 m_sectorSize;
		quint64 m_deviceSectors;

		QString m_deviceModel;
		QString m_deviceSerialNumber;

		QDateTime m_acquisitionDateTime;

		QString m_textNote;

		DfxmlVolume m_volume;

		QString m_md5HashDigest;
		QString m_sha1HashDigest;
};

#endif
