#include "opengl_window.hpp"

namespace hs
{
namespace recon
{

OpenGLWindow::OpenGLWindow(QWindow* parent)
  : QWindow(parent)
{
  setSurfaceType(QWindow::OpenGLSurface);
}

void OpenGLWindow::SetAnimating(bool animating)
{

}

void OpenGLWindow::RenderNow()
{

}

void OpenGLWindow::RenderLater()
{

}

bool OpenGLWindow::event(QEvent* event)
{
  return true;
}

void OpenGLWindow::exposeEvent(QExposeEvent* event)
{

}

}
}
