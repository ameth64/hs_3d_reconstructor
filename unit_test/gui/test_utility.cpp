#include "test_utility.hpp"

namespace test
{

bool IconCompare(const QIcon& icon_1, const QIcon& icon_2)
{
  for (int i = 0; i < 4; i++)
  {
    QIcon::Mode mode = QIcon::Mode(i);
    for (int j = 0; j < 2; j++)
    {
      QIcon::State state = QIcon::State(j);
      QList<QSize> sizes_1 = icon_1.availableSizes(mode, state);
      QList<QSize> sizes_2 = icon_2.availableSizes(mode, state);
      if (sizes_1.size() != sizes_2.size()) return false;
      for (int k = 0; k < sizes_1.size(); k++)
      {
        QSize size_1 = sizes_1[k];
        QSize size_2 = sizes_2[k];
        if (size_1 != size_2) return false;

        QPixmap pixmap_1 = icon_1.pixmap(size_1, mode, state);
        QPixmap pixmap_2 = icon_2.pixmap(size_2, mode, state);
        QImage image_1 = pixmap_1.toImage();
        QImage image_2 = pixmap_2.toImage();
        if (image_1 != image_2) return false;
      }
    }
  }

  return true;
}

}
