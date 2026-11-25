# Default recipe
default: build

# Get JlCxx prefix from Julia
jlcxx_prefix := `julia -e 'using CxxWrap; print(CxxWrap.prefix_path())'`

# Configure the build
setup:
    meson setup builddir --cmake-prefix-path="{{jlcxx_prefix}}"

# Build the project
build:
    meson compile -C builddir

# Run tests
test:
    meson test -C builddir

# Run tests with verbose output
test-verbose:
    meson test -C builddir --verbose

# Clean build directory
clean:
    rm -rf builddir

# Reconfigure from scratch
rebuild: clean setup build

# Install
install:
    meson install -C builddir

# Serve the web site (site/) locally with live reload. GODEBUG is unset
# because Go >= 1.24 binaries abort on the removed `tlskyber` setting.
site-serve:
    env -u GODEBUG hugo server --source site

# Build the web site into site/public/, failing on any warning
site-build:
    env -u GODEBUG hugo --source site --gc --minify --panicOnWarning
