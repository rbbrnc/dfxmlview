#ifndef DFXML_FILEOBJECT_TAG_H
#define DFXML_FILEOBJECT_TAG_H

#include <QList>
#include <QString>

#include "byterun.h"

class DfxmlFileObject
{
	public:
		DfxmlFileObject();

		void loadFromXmlStream(QXmlStreamReader &stream);
		void save(QXmlStreamWriter &stream);

		void addByteRun(const DfxmlByteRun &byteRun);


		QString fileName() const;
		void setFileName(const QString &name);

		quint64 fileSize() const;
		void setFileSize(const QString &size);

		QList<DfxmlByteRun> byteRunList() const;
		void setByteRuns(const QList<DfxmlByteRun> &byteRuns);

		QString hashDigest(const QString &type) const;
		void setHashDigest(const QString &type, const QString &hashdigest);

#ifndef QT_NO_DEBUG_OUTPUT
		void debugDump() const;
#endif

	private:
		QString m_fileName;
		quint64 m_fileSize;
		QList<DfxmlByteRun> m_byteRuns;

		QString m_md5HashDigest;
		QString m_sha1HashDigest;
};

#endif
