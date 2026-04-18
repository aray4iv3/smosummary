# Maintainer: aray4iv3 <aray.4iv3@gmail.com>
pkgname=smosummary
pkgver=1.0.0
pkgrel=1
pkgdesc="A Sum of Best (SoB) calculator for Super Mario Odyssey speedrunners"
arch=('x86_64')
url="https://github.com/aray4iv3/smosummary" # Change this to your actual link later
license=('MIT')
depends=('gcc-libs')
makedepends=('gcc')
source=("smosummary.cpp") # This assumes the .cpp file is in the same folder
sha256sums=('SKIP') # You can generate real sums later with 'updpkgsums'

build() {
  cd "$srcdir"
  # This is your "build script" right here!
  g++ -O3 smosummary.cpp -o smosummary
}

package() {
  # This tells Arch where to "install" the binary so you can run it from anywhere
  install -Dm755 smosummary "$pkgdir/usr/bin/smosummary"
}
