if ! cmake --version; then
    echo "Error: cmake is not installed.";
    exit 1;
fi

mkdir -p build && cd build && cmake -G "Ninja" ..
