import sqlite3
import logging
from datetime import datetime

logger = logging.getLogger(__name__)

DB_PATH = "agroedge.db"

def init_db():
    """Cria a tabela se não existir. Chamado uma vez na inicialização."""
    try:
        with sqlite3.connect(DB_PATH) as conn:
            conn.execute("""
                CREATE TABLE IF NOT EXISTS temperature_data (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    value REAL,
                    timestamp DATETIME
                )
            """)
            conn.commit()
        logger.info(f"Banco de dados inicializado em: {DB_PATH}")
    except sqlite3.Error as e:
        logger.error(f"Erro ao inicializar banco de dados: {e}")
        raise

def save_temperature(value):
    """Salva uma leitura de temperatura no banco."""
    try:
        with sqlite3.connect(DB_PATH) as conn:
            conn.execute(
                "INSERT INTO temperature_data (value, timestamp) VALUES (?, ?)",
                (value, datetime.now())
            )
            conn.commit()
    except sqlite3.Error as e:
        logger.error(f"Erro ao salvar temperatura no banco: {e}")
        raise
