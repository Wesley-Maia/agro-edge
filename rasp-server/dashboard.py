import dash
from dash import html, dcc, Input, Output
import plotly.express as px
import sqlite3
import pandas as pd

app = dash.Dash(__name__)

def load_data():
    conn = sqlite3.connect("agroedge.db")
    df = pd.read_sql_query("SELECT * FROM temperature_data", conn)
    conn.close()
    return df

app.layout = html.Div([
    html.H1("AgroEdge - Temperatura"),
    dcc.Graph(id="temp-graph"),
    dcc.Interval(
        id="interval",
        interval=10000,  # atualiza a cada 10 segundos
        n_intervals=0
    )
])

@app.callback(
    Output("temp-graph", "figure"),
    Input("interval", "n_intervals")
)
def update_graph(n):
    df = load_data()
    fig = px.line(df, x="timestamp", y="value")
    return fig

if __name__ == "__main__":
    app.run(debug=True)
    