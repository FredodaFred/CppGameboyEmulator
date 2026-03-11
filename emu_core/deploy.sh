echo "Running emulator core build..."
sh build.sh

echo "Copying binary to launcher"

TAURI_BIN_DIR="../emu_launcher/src-tauri/binaries"
mkdir -p "$TAURI_BIN_DIR"

# 3. Handle the Tauri Naming requirements
ARCH=$(uname -m)
if [ "$ARCH" = "arm64" ]; then
    SUFFIX="aarch64-apple-darwin"
else
    SUFFIX="x86_64-apple-darwin"
fi

cp ./build/src/GameBoyCpp "$TAURI_BIN_DIR/GameBoyCpp-$SUFFIX"

echo "✅ Success! Deployed to: $TAURI_BIN_DIR/GameBoyCpp-$SUFFIX"