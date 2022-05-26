"""
Launcher bez WSGI serveru
Vytvořil: Jakub Hlava (xhlava52)
"""
from itu_app import create_app

if __name__ == "__main__":
    app = create_app()
    app.app_context().push()
    app.run(debug=True, host="0.0.0.0", port=8080)

