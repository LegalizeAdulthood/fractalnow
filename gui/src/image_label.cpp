#include "image_label.h"
#include <QDebug>
#include <QPainter>

void ImageLabel::paintEvent(QPaintEvent *event)
{
	(void)event;

	if (image != 0) {
		QPainter painter(this);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		QRect rect(this->rect());
		if (image->rect().height() != 0 && rect.height() != 0) {
			double imageRatio = image->rect().width() / (double)image->rect().height();
			double rectRatio = rect.width() / (double)rect.height();
			if (rectRatio > imageRatio) {
				rect.setWidth(rect.height() * imageRatio);
			} else if (rectRatio < imageRatio) {
				rect.setHeight(rect.width() / imageRatio);
			}
			rect.moveCenter(this->rect().center());
		}
		painter.drawImage(rect, *image, image->rect());
	}
}

void ImageLabel::setImage(QImage *newImage)
{
	image = newImage;
	if (image != NULL) {
		resize(newImage->size());
	}
}
