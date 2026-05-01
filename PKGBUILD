# maintainer: aray4iv3 <aray.4iv3@gmail.com>
pkgname=smosummary
pkgver=3.0.1
pkgrel=3
pkgdesc="A Sum of Best (SoB) calculator and Stopwatch for Super Mario Odyssey speedrunners"
arch=('x86_64')
url="https://github.com/aray4iv3/smosummary"
license=('MIT')
depends=('gcc-libs')
makedepends=('rust' 'cargo')

source=("${pkgname}::git+${url}.git")
sha256sums=('SKIP')
provides=('smosummary')
conflicts=('smosummary-bin' 'smosummary-git')

prepare() {
  cd "$srcdir/$pkgname"
  cargo fetch --locked --target "$(rustc -vV | sed -n 's/host: //p')"
}

build() {
  cd "$srcdir/$pkgname"
  cargo build --release --frozen
}

package() {
  cd "$srcdir/$pkgname"
  install -Dm755 "target/release/$pkgname" "$pkgdir/usr/bin/$pkgname"
  install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
