import { applyRootStyles } from './src/utils.js';
import { GameBoard } from './src/game-board.js';
import { rootStyles, keyCodes } from './src/config.js';

applyRootStyles(rootStyles);
const gameBoard = new GameBoard(document.querySelector('#game-board'));
const scoreGame = document.querySelector('#score-value');
const highscoreGame = document.querySelector('#highscore');
const BASE_URL = 'http://188.19.14.103:8989';
const gameIds = [0, 1, 2];
let timerInterval; 
async function handleResponse(response) {
    if (response.ok) {
        return await response.json();
    } else {
        const errorData = await response.json();
        throw new Error(errorData.message);
    }
}

async function sendStartAction() {
    try {
        const response = await fetch(`${BASE_URL}/actions`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ action: 'start' })
        });
        await handleResponse(response);
        console.log('Действие start отправлено');
    } catch (error) {
        console.error('Не удалось отправить действие start:', error);
    }
}

async function startGame(gameId) {
    try {
        await sendStartAction();
        const response = await fetch(`${BASE_URL}/games/${gameId}`, { method: 'POST' });
        await handleResponse(response);
        console.log('Игра запущена');
        startTimer(); // Запуск таймера после начала игры
        fetchGameState(gameId);
    } catch (error) {
        console.error('Не удалось запустить игру:', error);
    }
}

async function fetchGameState(gameId) {
    try {
        const response = await fetch(`${BASE_URL}/state`);
        const gameState = await handleResponse(response);
        console.log('Текущее состояние игры:', gameState || 'Нет данных');
        gameBoard.update(gameState);
        scoreGame.textContent = gameState.score;
        highscoreGame.textContent = gameState.high_score;
    } catch (error) {
        console.error('Не удалось получить состояние игры:', error);
    }
}

function askForGameId() {
    let gameId;
    while (true) {
        gameId = prompt('Введите идентификатор игры (0, 1, 2):');
        if (gameIds.includes(Number(gameId))) {
            return Number(gameId);
        } else {
            alert('Неверный идентификатор. Пожалуйста, попробуйте еще раз.');
        }
    }
}

const selectedGameId = askForGameId();
startGame(selectedGameId);

setInterval(() => fetchGameState(selectedGameId), 50);

async function sendUserAction(actionId, hold) {
    try {
        const response = await fetch(`${BASE_URL}/actions`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ action_id: actionId, hold })
        });
        await handleResponse(response);
        console.log('Действие выполнено');
    } catch (error) {
        console.error('Не удалось отправить действие:', error);
    }
}

const actions = {
    ArrowUp: 5,
    ArrowRight: 4,
    ArrowDown: 6,
    ArrowLeft: 3,
    KeyP: 1,
    KeyQ: 2,
    KeyS: 0,
    KeyA: 7,
};

document.addEventListener('keydown', function (event) {
    const actionId = actions[event.code];
    if (actionId !== undefined) {
        sendUserAction(actionId, true);
        console.log(event.code.toLowerCase());
    }
});
