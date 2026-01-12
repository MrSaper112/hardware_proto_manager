#!/bin/bash
# Generate Doxygen documentation for GitHub Pages
set -e

cd "$(dirname "$0")"

echo "🔧 Generating Documentation for GitHub Pages..."

# Check dependencies
if ! command -v doxygen &> /dev/null; then
    echo "❌ Install doxygen: sudo apt-get install doxygen graphviz"
    exit 1
fi

# Generate docs
doxygen Doxyfile

# Create .nojekyll to prevent Jekyll processing
touch html/.nojekyll

echo ""
echo "✅ Documentation generated in docs/html/"
echo ""
echo "📋 GitHub Pages Setup:"
echo "   1. Go to: https://github.com/MrSaper112/hardware_proto_manager/settings/pages"
echo "   2. Source: Deploy from a branch"
echo "   3. Branch: Select your branch (main or dev/wrap_namespace)"
echo "   4. Folder: /docs"
echo "   5. Save"
echo ""
echo "📖 Your docs will be at: https://mrsaper112.github.io/hardware_proto_manager/html/"
