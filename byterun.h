#ifndef DFXML_BYTERUN_TAG_H
#define DFXML_BYTERUN_TAG_H

#include <QString>
#include <QList>
#include <QXmlStreamAttributes>

//    <byte_run offset='0' img_offset='9216' len='12288'/>
class DfxmlByteRun
{
	public:
		DfxmlByteRun();
		DfxmlByteRun(const QXmlStreamAttributes &attributes);

		void setImageOffset(const QString &offset);
		void setOffset(const QString &offset);
		void setSize(const QString &size);

		quint64 size() const;
		quint64 offset() const;
		quint64 imageOffset() const;

#ifndef QT_NO_DEBUG_OUTPUT
		void debugDump() const;
#endif

	private:
		quint64 m_offset;
		quint64 m_imageOffset;
		quint64 m_size;
};

#endif
