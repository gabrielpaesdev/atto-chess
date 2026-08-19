
bool isSquareAttacked(int8_t x, int8_t y, bool byWhite) {
    int8_t original = board[y][x];
    board[y][x] = byWhite ? -PAWN : PAWN;

    bool attacked = false;
    for (int8_t row = 0; row < BOARD_SIZE; ++row) {
        for (int8_t col = 0; col < BOARD_SIZE; ++col) {
            int8_t p = board[row][col];
            if (p == EMPTY || ((p > 0) != byWhite) || (row == y && col == x)) continue;
            if (isPseudoLegalMove(col, row, x, y)) { attacked = true; goto done; }
        }
    }
done:
    board[y][x] = original;
    return attacked;
}

bool isKingInCheck(bool whiteKing) {
    int8_t kingVal = whiteKing ? KING : -KING;
    for (int8_t y = 0; y < BOARD_SIZE; ++y)
        for (int8_t x = 0; x < BOARD_SIZE; ++x)
            if (board[y][x] == kingVal)
                return isSquareAttacked(x, y, !whiteKing);
    return false;
}

bool isLegalMove(int8_t startX, int8_t startY, int8_t endX, int8_t endY) {
    if (!isPseudoLegalMove(startX, startY, endX, endY)) return false;

    int8_t piece = board[startY][startX];

    if (abs8(piece) == KING && abs8(endX - startX) == 2) {
        if (isKingInCheck(piece > 0)) return false;
        int8_t midX = startX + ((endX > startX) ? 1 : -1);
        if (isSquareAttacked(midX, startY, piece < 0)) return false;
    }

    int8_t targetPiece      = board[endY][endX];
    int8_t enPassantCapture = EMPTY;

    if (abs8(piece) == PAWN && endX == epX && endY == epY) {
        enPassantCapture    = board[startY][endX];
        board[startY][endX] = EMPTY;
    }

    board[endY][endX]     = piece;
    board[startY][startX] = EMPTY;
    bool inCheck = isKingInCheck(piece > 0);

    board[startY][startX] = piece;
    board[endY][endX]     = targetPiece;
    if (enPassantCapture != EMPTY) board[startY][endX] = enPassantCapture;

    return !inCheck;
}

bool hasValidMoves(bool whiteTurn) {
    for (int8_t sy = 0; sy < BOARD_SIZE; ++sy)
        for (int8_t sx = 0; sx < BOARD_SIZE; ++sx) {
            int8_t p = board[sy][sx];
            if (p == EMPTY || ((p > 0) != whiteTurn)) continue;
            for (int8_t ey = 0; ey < BOARD_SIZE; ++ey)
                for (int8_t ex = 0; ex < BOARD_SIZE; ++ex)
                    if (isLegalMove(sx, sy, ex, ey)) return true;
        }
    return false;
}

void executeMove(int8_t startX, int8_t startY, int8_t endX, int8_t endY) {
    int8_t piece = board[startY][startX];
    int8_t type  = abs8(piece);

    if (type == PAWN && endX == epX && endY == epY)
        board[startY][endX] = EMPTY;

    epX = -1; epY = -1;
    if (type == PAWN && abs8(endY - startY) == 2) {
        epX = startX;
        epY = startY + ((piece > 0) ? -1 : 1);
    }

    if (type == PAWN && (endY == 0 || endY == 7))
        piece = (piece > 0) ? QUEEN : -QUEEN;

    if (type == KING && abs8(endX - startX) == 2) {
        if (endX == 6) { board[startY][5] = board[startY][7]; board[startY][7] = EMPTY; }
        if (endX == 2) { board[startY][3] = board[startY][0]; board[startY][0] = EMPTY; }
    }

    if (piece  ==  KING) castlingFlags |= 1;
    if (piece  == -KING) castlingFlags |= 8;
    if (startX == 0 && startY == 7) castlingFlags |= 2;
    if (startX == 7 && startY == 7) castlingFlags |= 4;
    if (startX == 0 && startY == 0) castlingFlags |= 16;
    if (startX == 7 && startY == 0) castlingFlags |= 32;

    board[endY][endX]     = piece;
    board[startY][startX] = EMPTY;
}
