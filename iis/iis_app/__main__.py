from iis_app import create_app, db, login_manager
from iis_app.model import *


if __name__ == "__main__":
    app = create_app()
    app.app_context().push()
    # this creates whole database model on first run and ensures, that missing parts of model are created on each run

    app.run(host="0.0.0.0", port=5555, debug=True)