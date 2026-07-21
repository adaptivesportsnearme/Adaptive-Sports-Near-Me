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
            <a hred="./index.php">Search</a>
            <a href="../about/index.html">About</a>
            <a href="#">Contact</a>
            <button class="nav-btn">List Your Program</button>

        </nav>
</header>

<main class="hero">

    
    <div class="hero-content">

        <div class="search-area">

            <h2>Explore our Database</h2>
            <!-- Search bar doesn't run any code yet, but eventually will run a jscript that displays database items in the appropriate order based on Clara's math, based on freshness, location, and filters-->
            <form class="search-form">
                <input
                    type="text"
                    class="search-input"
                    placeholder="Enter specifications..."
                >
                <select name="" id="">
                    <option value="1">Filter</option>
                </select>

                <button
                    type="submit"
                    class="search-btn">
                    Show Me Programs
                </button>
            </form>

        </div>
        <!--PHP time, This is a basic connection that connects the search page to the posgres database
        For each "instance" (which is basically each program) there will be a block made for the appropriate instance
        Currently the number of blocks isn't limited, but it will eventually be pages of 10 or 20 -->
        <?php $conn = pg_connect("host=localhost dbname=ASNM user=postgres password=");

$query = "SELECT * FROM ASNMInstances ORDER BY sport_name, instance_name";
$result = pg_query($conn, $query);
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

</body>
</html>