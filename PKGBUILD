# Maintainer: aray4iv3 <aray.4iv3@gmail.com>
pkgname=smosummary
pkgver=2.0.1
pkgrel=1
pkgdesc="A Sum of Best (SoB) calculator and Stopwatch for Super Mario Odyssey speedrunners"
arch=('x86_64')
url="https://github.com/aray4iv3/smosummary"
license=('MIT')
depends=('gcc-libs')
makedepends=('gcc')
source=("smosummary.cpp")
sha256sums=('SKIP')

build() {
  cd "$srcdir"
  # g++ script
  g++ -O3 smosummary.cpp -o smosummary
}

package() {
  # /usr/bin spot
  install -Dm755 smosummary "$pkgdir/usr/bin/smosummary"
}
