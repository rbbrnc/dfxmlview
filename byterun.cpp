#include <QDebug>

#include "byterun.h"

DfxmlByteRun::DfxmlByteRun()
	: m_offset(0),
	  m_imageOffset(0),
	  m_size(0)
{
}

DfxmlByteRun::DfxmlByteRun(const QXmlStreamAttributes &attributes)
{
	if (attributes.hasAttribute("offset")) {
	        setOffset(attributes.value("offset").toString());
	} else {
		qDebug() << "No <offset>";
	}

	if (attributes.hasAttribute("img_offset")) {
	        setImageOffset(attributes.value("img_offset").toString());
	} else {
			qDebug() << "No <img_offset>";
	}

	if (attributes.hasAttribute("len")) {
	        setSize(attributes.value("len").toString());
	} else {
		qDebug() << __PRETTY_FUNCTION__ << "No <len>";
	}
}

void DfxmlByteRun::setImageOffset(const QString &offset)
{
	bool ok;
        m_imageOffset = offset.toULongLong(&ok, 10);
	if (!ok) {
		qDebug() << "Invalid image_offset:" << offset;
		m_imageOffset = 0;
	}
}

void DfxmlByteRun::setOffset(const QString &offset)
{
	bool ok;
	m_offset = offset.toULongLong(&ok, 10);
	if (!ok) {
		qDebug() << "Invalid offset:" << offset;
		m_offset = 0;
	}
}

void DfxmlByteRun::setSize(const QString &size)
{
	bool ok;
	m_size = size.toULongLong(&ok, 10);
	if (!ok) {
		qDebug() << "Invalid len:" << size;
		m_size = 0;
	}
}

quint64 DfxmlByteRun::size() const
{
	return m_size;
}
quint64 DfxmlByteRun::imageOffset() const
{
	return m_imageOffset;
}

quint64 DfxmlByteRun::offset() const
{
	return m_offset;
}

#ifndef QT_NO_DEBUG_OUTPUT
void DfxmlByteRun::debugDump() const
{
	qDebug() << QString("<byte_run offset='%1' img_offset='%2' len='%3'/>")
			.arg(m_offset)
			.arg(m_imageOffset)
			.arg(m_size);

}
#endif
