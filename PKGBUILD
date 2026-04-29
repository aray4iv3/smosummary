# maintainer: aray4iv3 <aray.4iv3@gmail.com>
pkgname=smosummary-rust
pkgver=3.0.0
pkgrel=1
pkgdesc="A Sum of Best (SoB) calculator and Stopwatch for Super Mario Odyssey speedrunners which is smosummary, but in rust."
arch=('x86_64')
url="https://github.com/aray4iv3/smosummary"
license=('MIT')
depends=('gcc-libs')
makedepends=('rust' 'cargo')
source=("${pkgname}-${pkgver}.tar.gz::https://github.com/aray4iv3/smosummary/archive/v${pkgver}.tar.gz")
sha256sums=('SKIP')
provides=('smosummary')

build() {
  cd "$pkgname-$pkgver"
  cargo build --release --locked
}

package() {
  cd "$pkgname-$pkgver"
  install -Dm755 "target/release/smosummary" "$pkgdir/usr/bin/smosummary"
}
