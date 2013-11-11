#ifndef DFXML_VOLUME_TAG_H
#define DFXML_VOLUME_TAG_H

#include <QXmlStreamReader>
#include <QList>
#include <QString>

//    <volume>
//      <byte_runs>
//        <byte_run offset='0' img_offset='0' len='10485760'/>
//      </byte_runs>
//      <block_size>1024</block_size>
//    </volume>

#include "byterun.h"

class DfxmlVolume
{
	public:
		DfxmlVolume();

		void loadFromXmlStream(QXmlStreamReader &stream);
		void save(QXmlStreamWriter &stream);

		void addByteRun(DfxmlByteRun br);

		quint64 blockSize() const;
		void setBlockSize(const QString &name);

		QList<DfxmlByteRun> byteRunList() const;

#ifndef QT_NO_DEBUG_OUTPUT
		void debugDump() const;
#endif

	private:
		quint64 m_blockSize;
		QList<DfxmlByteRun> m_byteRuns;
};

#endif
