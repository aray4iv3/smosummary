# maintainer: aray4iv3 <aray.4iv3@gmail.com>
pkgname=smosummary
pkgver=3.0.0
pkgrel=2
pkgdesc="A Sum of Best (SoB) calculator and Stopwatch for Super Mario Odyssey speedrunners in rust."
arch=('x86_64')
url="https://github.com/aray4iv3/smosummary"
license=('MIT')
depends=('gcc-libs')
makedepends=('rust' 'cargo')
source=("${pkgname}-${pkgver}.tar.gz::https://github.com/aray4iv3/smosummary/archive/v${pkgver}.tar.gz")
sha256sums=('SKIP')
provides=('smosummary')

build() {
  cd "smosummary-$pkgver"
  cargo build --release
}

package() {
  cd "smosummary-$pkgver"  # And here
  install -Dm755 "target/release/smosummary" "$pkgdir/usr/bin/smosummary"
  install -Dm644 "LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
