#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

if ! command -v nc >/dev/null 2>&1; then
    echo "nc is required for integration tests." >&2
    exit 1
fi

if [[ ! -x ./build/main ]]; then
    echo "Expected ./build/main to exist. Run make first." >&2
    exit 1
fi

choose_port() {
    local port
    for _ in {1..50}; do
        port=$((RANDOM % 10000 + 20000))
        if ! nc -z 127.0.0.1 "$port" >/dev/null 2>&1; then
            echo "$port"
            return 0
        fi
    done
    return 1
}

PORT="$(choose_port)"
SERVER_LOG="$(mktemp)"
OUT="$(mktemp)"
PLAYER_NAME="TestUser_${RANDOM}${RANDOM}"
PLAYER_PASS="TestPass_${RANDOM}"

cleanup() {
    if [[ -n "${SERVER_PID:-}" ]] && kill -0 "$SERVER_PID" >/dev/null 2>&1; then
        kill "$SERVER_PID" >/dev/null 2>&1 || true
        wait "$SERVER_PID" >/dev/null 2>&1 || true
    fi
    rm -f "$OUT" "$SERVER_LOG"
    rm -f "./db/players/${PLAYER_NAME}"
}
trap cleanup EXIT

# Keep integration deterministic by rebuilding the starting room from scripts.
rm -f "./db/rooms/0_0"

GAUZARBEIT_SCRIPT_ROOT="tests/dat" ./build/main "$PORT" >"$SERVER_LOG" 2>&1 &
SERVER_PID=$!

for _ in {1..50}; do
    if nc -z 127.0.0.1 "$PORT" >/dev/null 2>&1; then
        break
    fi
    sleep 0.1
done

if ! nc -z 127.0.0.1 "$PORT" >/dev/null 2>&1; then
    echo "Server did not start on port $PORT." >&2
    exit 1
fi

{
    printf "quit\n"
    sleep 0.2
} | nc -w 2 127.0.0.1 "$PORT" >"$OUT"

grep -F "Goodbye!" "$OUT" >/dev/null
if grep -F "You need to log in!" "$OUT" >/dev/null; then
    echo "Anonymous quit should not require login." >&2
    exit 1
fi

{
    printf "look\n"
    sleep 0.3
    printf "register %s %s\n" "$PLAYER_NAME" "$PLAYER_PASS"
    sleep 0.3
    printf "quit\n"
} | nc -w 2 127.0.0.1 "$PORT" >"$OUT"

grep -F "Welcome to Gauzarbeit" "$OUT" >/dev/null
grep -F "You need to log in!" "$OUT" >/dev/null
grep -F "You are registered!" "$OUT" >/dev/null
grep -F "You are logged in as ${PLAYER_NAME}." "$OUT" >/dev/null

{
    printf "login %s %s\n" "$PLAYER_NAME" "$PLAYER_PASS"
    sleep 0.2
    printf "look here\n"
    sleep 0.2
    printf "quit\n"
} | nc -w 2 127.0.0.1 "$PORT" >"$OUT"

grep -F "You are logged in as ${PLAYER_NAME}." "$OUT" >/dev/null
grep -F "Room: Kitchen" "$OUT" >/dev/null

{
    printf "login %s wrongpass\n" "$PLAYER_NAME"
    sleep 0.2
} | nc -w 2 127.0.0.1 "$PORT" >"$OUT"

grep -F "Wrong password! Please try again." "$OUT" >/dev/null
if grep -F "[${PLAYER_NAME}@" "$OUT" >/dev/null; then
    echo "Failed login should not render a room status prompt." >&2
    exit 1
fi
