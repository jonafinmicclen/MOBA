#!/bin/bash
# launch.sh
#
# Starts the server, then one client (the "Game" executable - Client/MOBA
# are unused stub mains) per player listed in RuntimeData/game_args.json.
# Each client needs its own RuntimeData/auth_ctos.json carrying the account
# hash the server matches against game_args.json's player list (see
# ClientAuthSystem::authenticate) - since that path is hardcoded relative to
# the client's working directory, each client gets its own sandbox dir under
# .launch/ with a symlink to assets/ and RuntimeData/client_config.json (the
# other relative paths clients read, shared across every client) and its own
# auth_ctos.json.
#
# Ctrl+C (or any exit) kills the server and every client.

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${BUILD_DIR:-build}"
GAME_ARGS="$PROJECT_ROOT/RuntimeData/game_args.json"
LAUNCH_DIR="$PROJECT_ROOT/.launch"

SERVER_BIN="$PROJECT_ROOT/$BUILD_DIR/Server"
CLIENT_BIN="$PROJECT_ROOT/$BUILD_DIR/Game"

if [[ ! -x "$SERVER_BIN" ]]; then
    echo "Server binary not found at $SERVER_BIN (build it first, or set BUILD_DIR=build-release)" >&2
    exit 1
fi
if [[ ! -x "$CLIENT_BIN" ]]; then
    echo "Client binary not found at $CLIENT_BIN (build it first, or set BUILD_DIR=build-release)" >&2
    exit 1
fi

# Each player's "account" in game_args.json is exactly the hash value their
# client's auth_ctos.json must send (ClientAuthSystem::authenticate matches
# the two directly).
mapfile -t ACCOUNTS < <(python3 -c "
import json
with open('$GAME_ARGS') as f:
    args = json.load(f)
for p in args['players']:
    print(p['account'])
")

if [[ ${#ACCOUNTS[@]} -eq 0 ]]; then
    echo "No players found in $GAME_ARGS" >&2
    exit 1
fi

echo "Players (accounts): ${ACCOUNTS[*]}"

PIDS=()
cleanup() {
    echo "Shutting down..."
    for pid in "${PIDS[@]}"; do
        kill "$pid" 2>/dev/null || true
    done
}
trap cleanup EXIT
trap exit INT TERM

echo "Starting server..."
"$SERVER_BIN" &
PIDS+=("$!")

# Not strictly required (ServerConnectionManager retries on disconnect) but
# avoids a guaranteed-failed first connection attempt.
sleep 1

for account in "${ACCOUNTS[@]}"; do
    CLIENT_DIR="$LAUNCH_DIR/client_$account"
    mkdir -p "$CLIENT_DIR/RuntimeData"
    ln -sfn "$PROJECT_ROOT/assets" "$CLIENT_DIR/assets"
    ln -sfn "$PROJECT_ROOT/RuntimeData/client_config.json" "$CLIENT_DIR/RuntimeData/client_config.json"
    printf '{\n    "hash":"%s"\n}' "$account" > "$CLIENT_DIR/RuntimeData/auth_ctos.json"

    echo "Starting client for account $account..."
    (cd "$CLIENT_DIR" && exec "$CLIENT_BIN") &
    PIDS+=("$!")
done

wait
