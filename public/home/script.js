/**
    * Creates cells for the board and adds a listener to each one of them
    * 
    * **/
const createBoard = () => {
    for (let row = 0; row < boardSize; row++) {
        const tr = document.createElement('tr');
        for (let col = 0; col < boardSize; col++) {
          const td = document.createElement('td');
          // Color the cells in a checkerboard pattern
          if ((row + col) % 2 === 0) {
            td.classList.add('black');
          } else {
            td.classList.add('white');
          }

          // On click, send POST request to the server
          td.addEventListener('click', () => {
            handleCellClick(row, col);
          });

          // Give each <td> an ID so we can update it
          td.id = `cell-${row}-${col}`;
          tr.appendChild(td);
        }
        chessBoard.appendChild(tr);
      }
}

/**
    * Handles a click on a cell
    * **/
const handleCellClick = (row,col) => {
    console.log(`Clicked on a cell ${row} ${col}`);
}

const boardSize = 8;
const chessBoard = document.getElementById('chessBoard');
createBoard();
