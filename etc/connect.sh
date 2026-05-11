#!/usr/bin/env bash
set -euo pipefail

usage() {
    echo "Usage:"
    echo "$0 [container] [port]"
}

[ "${1:-}" = "-h" ] && usage && exit 0

selector="${1:-}"
port="${2:-23}"

resolve_container_id() {
    if [[ -n "$selector" ]]; then
        docker ps --format '{{.ID}}\t{{.Names}}' | awk -v s="$selector" '
            index($1, s) == 1 || $2 == s { print $1; exit }
        '
        return
    fi

    auto="$(docker ps --format '{{.ID}}\t{{.Names}}\t{{.Image}}' | awk '
        tolower($2) ~ /gauzarbeit|gzrbt/ || tolower($3) ~ /gauzarbeit|gzrbt/ { print $1; exit }
    ')"
    if [[ -n "$auto" ]]; then
        echo "$auto"
        return
    fi

    count="$(docker ps -q | wc -l | tr -d ' ')"
    if [[ "$count" == "1" ]]; then
        docker ps -q
        return
    fi
}

container_id="$(resolve_container_id || true)"
container_ip="$(docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' "$container_id")"

container_name="$(docker inspect -f '{{.Name}}' "$container_id" | sed 's#^/##')"
echo "Connecting to ${container_name} (${container_ip}:${port})..."
if command -v rlwrap >/dev/null 2>&1; then
    exec rlwrap -n -A -a -H /dev/null nc "$container_ip" "$port"
fi

exec nc "$container_ip" "$port"
