<!--Howdy! welcome to the search page of the ASNM webpage! This is the big one. If you want more general info go to the home page -->
<html lang="en">
      <head>   
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <!-- Bootstrap CSS -->
        <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css" rel="stylesheet">
        
        <!-- Google Fonts -->
        <link rel="preconnect" href="https://fonts.googleapis.com">
        <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
        <link href="https://fonts.googleapis.com/css2?family=Lexend+Deca:wght@100..900&family=Sigmar&display=swap" rel="stylesheet">
        <link rel="icon" type="image/x-icon" href="../img/asnm.jpg">
        <link rel="stylesheet" href="../style.css">
        <title>Search</title>
      </head>
      <body>
<!--Nav bar is identical to the home page -->
<header class="navbar">
        <div class="logo" >Adaptive Sports Near Me
        </div>
        <nav>
            <a href="../index.html">Home</a>
            <a href="./index.php">Search</a>
            <a href="../about/index.html">About</a>
            <a href="#">Contact</a>
            <button class="nav-btn">List Your Program</button>
        </nav>
</header>

<main class="hero">

    
    <div class="hero-content">

        <div class="search-area">

            <h2>Explore our Database</h2>
            <!-- This is a very very very basic searchbar. It doesn't have any autocorrect or guesser for if the user makes a typo
            Adding a "do you mean x? is the next step. It has a few different filters, those being All which looks through all fields
            Sport Name and Program Name, which are self explanitory, and City which is also pretty self explainatory-->
            <form class="search-form" method="GET" action="">
                <input
                    type="text"
                    name="search"
                    class="search-input"
                    placeholder="Enter specifications..."
                    value="<?= isset($_GET['search']) ? htmlspecialchars($_GET['search']) : '' ?>"
                >

                <select name="filter">
                    <option value="all">All</option>
                    <option value="sport">Sport Name</option>
                    <option value="program">Program Name</option>
                    <option value="city">City</option>
                </select>

                <button type="submit" class="search-btn">
                    Show Me Programs
                </button>
            </form>

        </div>
        <!--PHP time, This is a basic connection that connects the search page to the posgres database
        For each "instance" (which is basically each program) there will be a block made for the appropriate instance
        Currently the number of blocks isn't limited, but it will eventually be pages of 10 or 20 -->
        <!-- This is just a bunch of basic SQL queries that filters the results based on the filter and search query. It's very simplistic
        because it just does an ILIKE check, but it does work, so that's really cool -->
        <?php $conn = pg_connect("host=localhost dbname=ASNM user=postgres password=");

       
            $search = isset($_GET['search']) ? trim($_GET['search']) : "";
            $filter = isset($_GET['filter']) ? $_GET['filter'] : "all";

if ($search != "") {

    switch ($filter) {

        case "sport":
            $query = "SELECT * FROM ASNMInstances
                      WHERE sport_name ILIKE $1
                      ORDER BY sport_name, instance_name";
            break;

        case "program":
            $query = "SELECT * FROM ASNMInstances
                      WHERE instance_name ILIKE $1
                      ORDER BY sport_name, instance_name";
            break;

        case "city":
            $query = "SELECT * FROM ASNMInstances
                      WHERE instance_city ILIKE $1
                      ORDER BY sport_name, instance_name";
            break;

        default:
            $query = "SELECT * FROM ASNMInstances
                      WHERE
                          sport_name ILIKE $1 OR
                          instance_name ILIKE $1 OR
                          instance_city ILIKE $1 OR
                          instance_desc ILIKE $1
                      ORDER BY sport_name, instance_name";
    }

    $result = pg_query_params($conn, $query, array("%$search%"));

} else {

    $query = "SELECT * FROM ASNMInstances
              ORDER BY sport_name, instance_name";

    $result = pg_query($conn, $query);
}
        ?>

       <div class="results">
<!--for every row -->
        <?php while ($row = pg_fetch_assoc($result)) { ?>

    <div class="sport-card">
<!-- Show the name of the program-->
        <h3><?= htmlspecialchars(trim($row['instance_name'])) ?></h3>
<!-- show the name of the sport -->
        <h4><?= htmlspecialchars(trim($row['sport_name'])) ?></h4>
        <!-- x to y for ages-->
        <a>
            Ages: <?= $row['instance_age_floor'] ?> - <?= $row['instance_age_ceil'] ?>
        </a>
        <!--description of the program, limited at 200 chars, might be made into 400 -->
        <h5>
            <?= htmlspecialchars(trim($row['instance_desc'])) ?>
        </h5>
        <!-- All of the address information is shown-->
        <a>
            <?= htmlspecialchars(trim($row['instance_address_line_1'])) ?><br>

            <?= htmlspecialchars(trim($row['instance_city'])) ?>,
            <?= htmlspecialchars(trim($row['instance_state'])) ?>
            <?= htmlspecialchars(trim($row['instance_zip'])) ?>

            <br><br>
        <!--Start and end dates for the program -->
            <?= $row['instance_start_month'] ?>/<?= $row['instance_start_day'] ?>/<?= $row['instance_start_year'] ?>
            -

            <?= $row['instance_end_month'] ?>/<?= $row['instance_end_day'] ?>/<?= $row['instance_end_year'] ?>
        </a>

        <h6>
            <!--This is the skill level section, NULL is the false state here, if the attribute is NULL then it doesn't allow that skill level
            and then displays what the database says -->
            <?php
            $levels = [];

            if ($row['instance_allows_beginners'])
                $levels[] = "Beginner";

            if ($row['instance_allows_intermediate'])
                $levels[] = "Intermediate";

            if ($row['instance_allows_advanced'])
                $levels[] = "Advanced";

            echo implode(", ", $levels);
            ?>

            <br>
            <!--Cost of the program -->
            Cost:
            <?= ($row['instance_cost'] == 0)
                ? "Free"
                : "$" . number_format($row['instance_cost'], 2); ?>

        </h6>
        <!--Everything else is pretty self explainatory -->
        <a>
            Organization ID:
            <?= htmlspecialchars(trim($row['organization_id'])) ?>
        </a>

        <h6>
            Quality Rating:
            <?= number_format($row['instance_quality'], 1) ?>/5
        </h6>

    </div>

<?php } ?>

</div>

    </div>

</main>
<section class="newsletter">
    <div class="container newsletter-inner">

        <div>
            <h3>Stay in the loop</h3>
            <p>Get notified when new programs are added near you.</p>
        </div>

        <form class="newsletter-form">
            <input type="email" placeholder="you@example.com">
            <button type="submit">Subscribe →</button>
        </form>

    </div>
</section>

</body>
</html>